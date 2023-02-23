pub mod errors;

use std::collections::HashMap;
use std::io::{Read, Write};
use std::path::Path;

use crate::data::game::{Game, GameBuilder, IntermediateGame, TeamValue};
use crate::data::gameinfo::InternalGameInfo;
use crate::data::stats::{GoalBuilder, GoalStrength, Period, PowerPlay, Shots, Time};
use crate::data::team::get_id;
use crate::processing::{check_missing, verify_deserialized_content};
use crate::{scrape_and_log, scrape_gameinfos, BASE_URL, GAMES_IN_SEASON};
use chrono::Datelike;
use reqwest::blocking::Client;

use crate::scrape::errors::BuilderError;
use std::convert::TryFrom;

pub type ScrapeResults<T> = Result<T, (usize, BuilderError)>;
pub type GameResult = Result<Game, BuilderError>;

pub const _BASE: &'static str = "https://www.nhl.com/gamecenter/";
pub const _VS: &'static str = "-vs-";

pub struct GameInfoScraper<'a> {
  game_info_file: std::fs::File,
  db_path: &'a Path,
}

pub struct GameResultScraper<'a> {
  game_infos: Vec<InternalGameInfo>,
  db_path: &'a Path,
}

pub struct GameResults<'a> {
  games: Vec<Game>,
  scraped_games: usize,
  serialized_games: usize,
  db_path: &'a Path,
}

impl<'a> GameResults<'a> {
  pub fn serialize(&self) {
    let mut game_results = std::fs::OpenOptions::new()
      .read(true)
      .write(true)
      .create(true)
      .truncate(true) // we don't want to append. we want to clear and re-write + add
      .open(self.db_path.join("gameresults.db"))
      .expect(
        format!(
          "Couldn't open/create file {}",
          self.db_path.join("gameinfo.db").display()
        )
        .as_ref(),
      );
    let data = serde_json::to_string(&self.games).expect("Couldn't serialize game results data");
    match game_results.write_all(data.as_bytes()) {
      Ok(_) => {
        println!(
          "Successfully wrote serialized data to file {} bytes with {} new games. Total db: {} games",
          data.as_bytes().len(),
          self.scraped_games,
          self.games.len()
        );
      }
      Err(e) => {
        panic!("Could not write serialized data to file: {}", e);
      }
    }
  }
}

impl<'a> std::fmt::Display for GameResults<'a> {
  fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
    write!(
      f,
      "Scraped games: {}. Serialized games: {}",
      self.scraped_games, self.serialized_games
    )
  }
}

impl<'a> GameInfoScraper<'a> {
  pub fn new(db_path: &'a Path) -> GameInfoScraper {
    let game_info_file = std::fs::OpenOptions::new()
      .read(true)
      .write(true)
      .create(true)
      .open(&Path::new(db_path).join("gameinfo.db"))
      .expect(
        format!(
          "Couldn't open/create file {}",
          &Path::new(db_path).join("gameinfo.db").display()
        )
        .as_ref(),
      );

    GameInfoScraper {
      game_info_file,
      db_path,
    }
  }

  fn check_game_info_db_contents(&mut self) -> Option<Vec<InternalGameInfo>> {
    let mut game_info_buf = String::with_capacity(120000);
    self
      .game_info_file
      .read_to_string(&mut game_info_buf)
      .ok()
      .and_then(|bytes| {
        println!("read {} bytes from game info db", bytes);
        serde_json::from_str(&game_info_buf)
          .ok()
          .and_then(|gameinfos| {
            if verify_deserialized_content(&gameinfos) {
              Some(gameinfos)
            } else {
              None
            }
          })
      })
  }

  pub fn scrape_game_infos(mut self) -> GameResultScraper<'a> {
    if let Some(game_infos) = self.check_game_info_db_contents() {
      GameResultScraper {
        game_infos,
        db_path: self.db_path,
      }
    } else {
      let partials_dir = self.db_path.join("gi_partials");
      if !partials_dir.exists() {
        match std::fs::create_dir(&partials_dir) {
          Ok(()) => println!("partials dir created..."),
          Err(err) => match err.kind() {
            std::io::ErrorKind::AlreadyExists => println!("partials dir exists - continuing"),
            _ => panic!("Failed to create partials dir {}", err),
          },
        };
      }
      let mut all_partials = Vec::new();
      let dir_iterator = std::fs::read_dir(&partials_dir)
        .expect(format!("Couldn't read directory {} or it's contents", partials_dir.display()).as_ref());

      for entry in dir_iterator {
        let file_entry = entry.expect("Couldn't unwrap iterator for file");
        let mut buf = String::new();
        let mut f_handle = std::fs::OpenOptions::new()
          .read(true)
          .write(false)
          .create(false)
          .open(file_entry.path())
          .expect(format!("Couldn't open file {} for processing", file_entry.path().display()).as_ref());
        let bytes_read = f_handle.read_to_string(&mut buf).expect(
          format!(
            "Couldn't read file contents of {} to char buffer for processing",
            file_entry.path().display()
          )
          .as_ref(),
        );
        if bytes_read <= 2 { // means we have EOF or just empty [] json array or empty json object {}
           // skip this file
        } else {
          let partials: Vec<InternalGameInfo> = serde_json::from_str(&buf).expect(
            format!(
              "Couldn't de-serialize contents from file {}",
              file_entry.path().display()
            )
            .as_ref(),
          );
          all_partials.extend(partials);
        }
      }
      let game_infos = if let Some(missing) = check_missing(&all_partials) {
        let missing_scraped = scrape_gameinfos(self.db_path, missing);
        all_partials.extend(missing_scraped.into_iter());
        all_partials.sort_by(|a, b| a.get_id().cmp(&b.get_id()));
        let de_duplicated: HashMap<usize, &InternalGameInfo> =
          all_partials.iter().map(|val| (val.get_id(), val)).collect();
        assert_eq!(de_duplicated.len(), all_partials.len());
        all_partials
      } else {
        all_partials
      };
      assert_eq!(game_infos.len(), GAMES_IN_SEASON);
      let data =
        serde_json::to_string(&game_infos).expect("Could not de-serialize fully compiled Game Info db to file");

      match self.game_info_file.write_all(data.as_bytes()) {
        Ok(_) => println!("Successfully wrote serialized data of fully compiled Game Info db"),
        Err(e) => panic!("Failed to write game info: {}", e),
      }
      match std::fs::remove_dir_all(&partials_dir) {
        Ok(_) => println!("Cleaned up partials directory..."),
        Err(_) => println!("Could not clean up partials directory..."),
      }
      GameResultScraper {
        game_infos,
        db_path: self.db_path,
      }
    }
  }
}

impl<'a> GameResultScraper<'a> {
  pub fn scrape_games(self) -> GameResults<'a> {
    let mut game_results = std::fs::OpenOptions::new()
      .read(true)
      .write(true)
      .create(true)
      .open(&self.db_path.join("gameresults.db"))
      .expect(
        format!(
          "Couldn't open/create file {}",
          &self.db_path.join("gameresults.db").display()
        )
        .as_ref(),
      );
    let mut buf = String::new();
    let mut scraped_games: Vec<Game> = game_results
      .read_to_string(&mut buf)
      .ok()
      .and_then(|bytes| {
        println!("read {} bytes from game results db", bytes);
        let games_ = serde_json::from_str::<Vec<IntermediateGame>>(&buf);
        match games_ {
            Ok(games) =>{
              println!("deserialized {} games", games.len());
              Some(games)
            },
            Err(err) => {
              println!("Could not deserialize games: {}", err);
              None
            },
        }
      })
      .map(|games| games.into_iter().map(|g| Game::from(g)).collect())
      .unwrap_or(vec![]);

    println!("De-serialized {} games", scraped_games.len());
    let today = chrono::Utc::now();
    let date_tuple = (today.day(), today.month(), today.year() as u32);

    let refs = self
      .game_infos
      .iter()
      .filter(|g| {
        g.is_played(date_tuple)
          && scraped_games
            .iter()
            .find(|game| game.id() == g.get_id())
            .is_none()
      })
      .map(|x| x)
      .collect();
    let (game_results, errors) = scrape_and_log(&refs);
    let serialized_games = scraped_games.len();
    let scraped_games_count = game_results.len();
    scraped_games.extend(game_results);
    scraped_games.sort_by(|a, b| a.id().cmp(&b.id()));
    if !errors.is_empty() {
      println!("Error trying to scrape {} games", errors.len());
      for (id, err) in errors {
        println!("Game {} could not be scraped: {:?}", id, err);
      }
    }

    GameResults {
      games: scraped_games,
      scraped_games: scraped_games_count,
      serialized_games: serialized_games,
      db_path: self.db_path,
    }
  }
}

pub fn convert_fwd_slashes(ch: char) -> char {
  if ch == '/' {
    '-'
  } else {
    ch
  }
}

/// Returns tuple of PowerPlay stats from game, where the returning tuple contains (away, home) stats
fn process_pp_summary(penalty_summary: &Vec<String>) -> (TeamValue<PowerPlay>, TeamValue<PowerPlay>) {
  let (a_pps, _) = penalty_summary[0].split_at(
    penalty_summary[0]
      .find("/")
      .expect("Data for Power plays is not formatted correctly"),
  );
  let (h_pps, _) = penalty_summary[1].split_at(
    penalty_summary[1]
      .find("/")
      .expect("Data for Power plays is not formatted correctly"),
  );

  let a_data = a_pps.split("-").collect::<Vec<&str>>();
  let h_data = h_pps.split("-").collect::<Vec<&str>>();
  let (a_goals, a_total) = (
    a_data[0]
      .parse::<u32>()
      .expect(format!("Could not parse PP goals (Away): {}", a_data[0]).as_ref()),
    a_data[1]
      .parse::<u32>()
      .expect(format!("Could not parse PP total (Away): {}", a_data[1]).as_ref()),
  );
  let (h_goals, h_total) = (
    h_data[0]
      .parse::<u32>()
      .expect(format!("Could not parse PP goals (Home): {}", h_data[0]).as_ref()),
    h_data[1]
      .parse::<u32>()
      .expect(format!("Could not parse PP total (Home): {}", h_data[1]).as_ref()),
  );
  let away_pp = TeamValue::Away(PowerPlay {
    goals: a_goals,
    total: a_total,
  });
  let home_pp = TeamValue::Home(PowerPlay {
    goals: h_goals,
    total: h_total,
  });
  (away_pp, home_pp)
}

fn scrape_game(client: &reqwest::blocking::Client, game_info: &InternalGameInfo) -> GameResult {
  use select::document::Document;
  use select::predicate::{And, Attr, Class, Name};
  let (gs, evt, sh) = (
    client.get(&game_info.get_game_summary_url()).send(),
    client.get(&game_info.get_event_summary_url()).send(),
    client.get(&game_info.get_shot_summary_url()).send(),
  );

  let (game_html_data, event_html_data, shots_html_data) = match (gs, evt, sh) {
    (Ok(a), Ok(b), Ok(c)) => (a.text().unwrap(), b.text().unwrap(), c.text().unwrap()),
    (Err(e), _, _) => {
      return Err(BuilderError::REQWEST(e));
    }
    (_, Err(e), _) => {
      return Err(BuilderError::REQWEST(e));
    }
    (_, _, Err(e)) => return Err(BuilderError::REQWEST(e)),
  };

  let evt_doc = Document::from(event_html_data.as_ref());
  let game_doc = Document::from(game_html_data.as_ref());
  let shots_doc = Document::from(shots_html_data.as_ref());
  let evt_pred = And(Class("bold"), Name("tr"));
  let gs_table_predicate = Name("table");

  let mut gb = GameBuilder::new();
  gb.game_info(game_info.clone());

  let event_nodes: Vec<select::node::Node> = evt_doc
    .find(evt_pred)
    .filter(|n| n.text().contains("TEAM TOTALS"))
    .map(|x| x.clone())
    .collect();

  for (index, node) in event_nodes.iter().enumerate() {
    for (i, stat) in node.find(Name("td")).enumerate().skip(1) {
      match i {
        // 1 if index == 0 => gb.final_score(TeamValue::Away(stat.text().parse::<usize>().expect("Couldn't parse away score"))),
        // 1 if index == 1 => gb.final_score(TeamValue::Home(stat.text().parse::<usize>().expect("Couldn't parse away score"))),
        5 => {}  // PN (Number of Penalties)
        6 => {}  // PIM (Penalty Infraction Minutes)
        13 => {} // Shots
        17 if index == 0 => gb.give_aways(TeamValue::Away(
          stat
            .text()
            .parse::<usize>()
            .expect("Could not parse give aways for away team"),
        )), // GV Give aways
        17 if index == 1 => gb.give_aways(TeamValue::Home(
          stat
            .text()
            .parse::<usize>()
            .expect("Could not parse give aways for home team"),
        )),
        18 if index == 0 => gb.take_aways(TeamValue::Away(
          stat
            .text()
            .parse::<usize>()
            .expect("Could not parse give aways for away team"),
        )),
        18 if index == 1 => gb.take_aways(TeamValue::Home(
          stat
            .text()
            .parse::<usize>()
            .expect("Could not parse give aways for home team"),
        )),
        22 if index == 0 => gb.face_offs(TeamValue::Away(
          stat
            .text()
            .parse::<f32>()
            .expect("Couldn't parse face off value for away team"),
        )), // Faceoff win %
        22 if index == 1 => gb.face_offs(TeamValue::Home(
          stat
            .text()
            .parse::<f32>()
            .expect("Couldn't parse face off value for home team"),
        )),
        // F% Faceoff win percentage
        _ => {}
      }
    }
  }
  // skip header table (1st)
  for node in game_doc.find(gs_table_predicate).skip(9).take(1) {
    // skip goals made header row (1st)
    for tr_node in node.find(Name("tr")).skip(1) {
      let mut goal_builder = GoalBuilder::new();
      let mut period = String::new();
      for (td_index, goal_node) in tr_node.find(Name("td")).enumerate() {
        let nodestr = goal_node.text().trim().to_owned();
        match td_index {
          0 if nodestr == "-" => {
            goal_builder.was_unsuccessful_penalty_shot();
          }
          1 => {
            period = nodestr;
          }
          2 => {
            if period == "SO" {
              let p = Period::new(&period, None).expect("Could not parse period");
              goal_builder.period(p);
            } else {
              let time_components: Vec<&str> = nodestr.split(":").collect();
              let (min, sec) = (
                time_components[0]
                  .parse::<u16>()
                  .expect(format!("Could not parse minutes: {}", nodestr).as_ref()),
                time_components[1]
                  .parse::<u16>()
                  .expect("Could not parse seconds"),
              );
              let time = Time::new(min, sec);
              let p = Period::new(&period, Some(time)).expect("Could not parse period");
              goal_builder.period(p)
            }
          }
          3 => {
            if !goal_builder.is_shootout() {
              let strength = GoalStrength::try_from(&nodestr)
                .ok()
                .expect("Could not parse strength");
              goal_builder.strength(strength);
            } else {
              goal_builder.strength(GoalStrength::Shootout);
            }
          }
          4 => {
            let team_id =
              get_id(&nodestr).expect(format!("Could not find a team with that name: {}", &nodestr).as_ref());
            goal_builder.team(team_id);
          }
          5 => {
            goal_builder.player(nodestr);
          }
          6 | 7 => {
            goal_builder.assist(nodestr);
          }
          _ => {}
        }
      }
      gb.add_goal(goal_builder);
    }
  }

  gb.set_final_score();
  let mut penalty_summary = vec![String::from("TOT (PN-PIM)")];
  game_doc.find(Attr("id", "PenaltySummary")).for_each(|v| {
    let s = v.text();
    let mut totals = 0;
    let collected: Vec<&str> = s
      .lines()
      .rev()
      .take_while(|line| {
        if line.contains("TOT") {
          totals += 1;
          if totals == 2 {
            return false;
          }
        }
        return true;
      })
      .collect();

    let values: Vec<&str> = collected
      .into_iter()
      .rev()
      .filter(|str| str.len() >= 2 && *str != "\u{a0}")
      .collect();

    for item in values {
      penalty_summary.push(item.to_owned());
    }
  });

  let indicator = "Power Plays (Goals-Opp./PPTime)";
  let mut indices = vec![];

  for (index, item) in penalty_summary.iter().enumerate() {
    if item == indicator {
      indices.push(index + 1);
    }
  }

  let data = indices
    .iter()
    .map(|index| penalty_summary.get(*index).unwrap().clone())
    .collect();

  let (away_pp, home_pp) = process_pp_summary(&data);
  gb.power_plays(away_pp);
  gb.power_plays(home_pp);

  let shots_table = shots_doc.find(Attr("id", "ShotsSummary")).next().unwrap();
  let mut home_shots = Vec::new();
  let mut away_shots = Vec::new();
  for (table_index, node) in shots_table.find(Name("table")).enumerate().skip(3).take(6) {
    match table_index {
      3 => {
        let data = node.text();
        let s: Vec<&str> = data.lines().filter(|line| line.len() != 0).collect();
        let mut periods = Vec::new();
        for row in s.chunks(5).skip(1) {
          periods.push(row[4].trim());
        }
        periods.pop(); // Make sure we remove the TOTALS row
        for (_, shot_stats) in periods.iter().enumerate() {
          if shot_stats.is_empty() {
            away_shots.push(0);
          } else {
            // println!("shot_stats: {}", shot_stats);
            let (_goals, shots) = shot_stats.split_at(shot_stats.find("-").unwrap());
            let _shots = shots[1..].parse::<usize>().expect("Couldn't parse shots");
            away_shots.push(_shots);
          }
        }
      }
      8 => {
        let data = node.text();
        let s: Vec<&str> = data.lines().filter(|line| line.len() != 0).collect();
        let mut periods = Vec::new();
        for row in s.chunks(5).skip(1) {
          periods.push(row[4].trim());
        }
        periods.pop(); // Make sure we remove the TOTALS row
        for (_period_index, shot_stats) in periods.iter().enumerate() {
          if shot_stats.is_empty() {
            home_shots.push(0);
          } else {
            let (_goals, shots) = shot_stats.split_at(shot_stats.find("-").unwrap());
            let _shots = shots[1..].parse::<usize>().expect("Couldn't parse shots");
            home_shots.push(_shots);
          }
        }
      }
      _ => {}
    }
  }

  let shots: Vec<Shots> = away_shots
    .into_iter()
    .zip(home_shots.into_iter())
    .map(|(away, home)| Shots { away, home })
    .collect();
  gb.shots(shots);
  gb.finalize()
}

pub fn scrape_game_results(thread_count: usize, games: &Vec<&InternalGameInfo>) -> Vec<ScrapeResults<Game>> {
  println!("Running game scraping...");
  use pbr::ProgressBar;

  let per_thread_work_count = (games.len() as f32 / thread_count as f32).ceil() as usize;
  let mut per_thread_work: Vec<Vec<InternalGameInfo>> = games
    .chunks(per_thread_work_count)
    .map(|slice| {
      let mut res = Vec::with_capacity(slice.len());
      for &i in slice {
        res.push(i.clone());
      }
      res
    })
    .collect();
  let (tx, rx) = std::sync::mpsc::channel();
  let mut jobs = vec![];
  for _ in 0..thread_count {
    if let Some(work) = per_thread_work.pop() {
      let tx_clone = tx.clone();
      let job = std::thread::spawn(move || {
        let work = work;
        let client = Client::new();
        let mut result = Vec::new();
        for game_info in work {
          let res = scrape_game(&client, &game_info);
          match res {
            Ok(game) => {
              result.push(Ok(game));
            }
            Err(e) => {
              result.push(Err((game_info.get_id(), e)));
            }
          }
          let _ = tx_clone.send(1);
        }
        drop(tx_clone);
        result
      });
      jobs.push(job);
    }
  }
  drop(tx);
  let mut result = Vec::new();
  let mut pb = ProgressBar::new(games.len() as u64);
  pb.format("╢▌▌░╟");
  for e in rx {
    pb.add(e);
  }

  for job in jobs {
    match job.join() {
      Ok(res) => result.extend(res),
      Err(e) => panic!("Failed to scrape game data: {:?}", e),
    }
  }
  pb.finish_print(format!("Done scraping game results for {} games.", games.len()).as_ref());
  result
}

pub fn scrape_game_infos(thread_count: usize, game_ids: &Vec<usize>) -> Vec<ScrapeResults<InternalGameInfo>> {
  // Rust ranges are end-exclusive. So we have to add 1
  use pbr::ProgressBar;
  let count = game_ids.len() as u64;
  let mut pb = ProgressBar::new(count);
  pb.format("╢▌▌░╟");

  let per_thread_work_count = (game_ids.len() as f32 / thread_count as f32).ceil() as usize;
  let mut per_thread_work: Vec<Vec<usize>> = game_ids
    .chunks(per_thread_work_count)
    .map(|slice| slice.into())
    .collect();
  let (tx, rx) = std::sync::mpsc::channel();
  let mut jobs = vec![];
  for _ in 0..thread_count {
    // if we for instance have 4 threads, and we got 3 items to scrape
    // this will be spread out over 3 threads (not ideal), but it will at least
    // succeed, since the 4th thread won't attempt to pop work items that don't exist
    if let Some(work) = per_thread_work.pop() {
      let tx_clone = tx.clone();
      let t = std::thread::spawn(move || {
        let work = work;
        let client = Client::new();
        // we move work into the thread
        let mut result = Vec::with_capacity(work.len());
        for id in work {
          let url_string = format!("{}/{}", BASE_URL, id);
          let r = client.get(&url_string).send();
          if let Ok(resp) = r {
            let url = resp.url();
            let g_info_result = InternalGameInfo::from_url(url);
            match g_info_result {
              Ok(res) => result.push(Ok(res)),
              Err(e) => result.push(Err((id, e))),
            }
          } else if let Err(e) = r {
            result.push(Err((id, BuilderError::from(e))));
          }
          let _ = tx_clone.send(1);
        }
        // we want to drop the transmitter/sender, because we want the for loop to end, when all senders are closed
        drop(tx_clone);
        result
      });
      jobs.push(t);
    }
  }
  // we want to drop the transmitter/sender, because we want the for loop to end, when all senders are closed
  drop(tx);

  for e in rx {
    pb.add(e);
  }

  let mut result = Vec::with_capacity(game_ids.len());
  for job in jobs {
    let scraped = job.join();
    match scraped {
      Ok(res) => result.extend(res),
      Err(err) => panic!("Failed to get results from worker thread: {:?}", err),
    }
  }

  pb.finish_print(format!("Done scraping game info for {} games.", result.len()).as_ref());
  result
}

pub fn process_results(
  results: &Vec<ScrapeResults<InternalGameInfo>>,
) -> (Vec<&InternalGameInfo>, Vec<&(usize, BuilderError)>) {
  let errors: Vec<&(usize, BuilderError)> = results.iter().filter_map(|f| f.as_ref().err()).collect();
  let games: Vec<&InternalGameInfo> = results.iter().filter_map(|f| f.as_ref().ok()).collect();
  (games, errors)
}
