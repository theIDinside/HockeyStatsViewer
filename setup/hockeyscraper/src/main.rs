// TODOS / Description of application's process
// Pre-requisite data:
// - Schedule
// Get today's date, compare to downloaded schedule
// Check latest added game, and it's gameID
// Compare latest scraped ID, with first game of today's ID
// Scrape the data between last saved, and first game of today

// Possible structs:
//  GameInfo - keeps record of home team, away team, date and gameID of a game
//  Game - The actual scraped data of the result of a game
extern crate chrono;
extern crate reqwest;
extern crate scraper;
extern crate select;
extern crate serde;
extern crate serde_json;
#[macro_use]
extern crate serde_derive;
extern crate pbr;

mod data;
mod processing;
mod scrape;

use std::collections::HashMap;
use std::fs::OpenOptions;
use std::io::{Read, Write};
use std::path::Path;
use std::time::Instant;

use chrono::Datelike;
use data::{
  game::Game,
  gameinfo::InternalGameInfo,
  team::{construct_league, write_league_to_file},
};
use processing::GameInfoScraped;
use scrape::errors::BuilderError;
use scrape::{process_results, scrape_game_infos};

use crate::data::game::ScrapedSeason;

const BASE_URL: &'static str = "https://www.nhl.com/gamecenter";

const FIRST_GAME: usize = 2022020001;
const GAMES_IN_SEASON: usize = 1312;
const DB_DIR: &'static str = "./assets/db";

impl processing::FileString for std::fs::File {
  fn string_read(&mut self) -> processing::FileResult {
    let mut buf = String::new();
    self.read_to_string(&mut buf)?;
    Ok(buf)
  }
}

/// Utility wrapper around scraping, so that main() doesn't get cluttered with calls to println!
fn scrape_and_log(games: &Vec<&InternalGameInfo>) -> (Vec<Game>, Vec<(usize, BuilderError)>) {
  println!("Beginning scraping of {} games", games.len());
  let mut ok_games = vec![];
  let mut errs = vec![];

  let scrape_begin_time = Instant::now();
  let threads: usize = std::thread::available_parallelism()
    .expect("Failed to get cpus?")
    .try_into()
    .expect("Failed to convert");
  println!("Attempting to use {} threads...", threads);
  let game_results = scrape::scrape_game_results(threads, &games);
  let count = game_results.len();
  for g in game_results {
    match g {
      Ok(game) => ok_games.push(game),
      Err(err) => errs.push(err),
    }
  }
  let scrape_time_elapsed = scrape_begin_time.elapsed().as_millis();
  println!("Tried scraping {} games in {}ms", count, scrape_time_elapsed);
  (ok_games, errs)
}

pub const FULLSEASON: std::ops::Range<usize> = FIRST_GAME..(FIRST_GAME + GAMES_IN_SEASON);

pub fn game_info_scrape_all() {
  // Begin scraping of all game info
  let full_season_ids: Vec<usize> = (FIRST_GAME..(FIRST_GAME + GAMES_IN_SEASON)).collect();
  println!("There is no saved Game Info data. Begin scraping of Game Info DB...");
  // We split the games into 100-game chunks, so if anything goes wrong, we at least write 100 games to disk at a time
  let game_id_chunks: Vec<Vec<usize>> = full_season_ids
    .chunks(100)
    .map(|chunk| chunk.into_iter().map(|v| *v).collect())
    .collect();
  let threads: usize = std::thread::available_parallelism()
    .expect("Failed to get cpus?")
    .try_into()
    .expect("Failed to convert");
  println!("Attempting to use {} threads...", threads);
  for (index, game_ids) in game_id_chunks.iter().enumerate() {
    println!(
      "Scraping game info for games {}-{}",
      game_ids[0],
      game_ids[game_ids.len() - 1]
    );
    let file_name = format!("gameinfo_partial-{}.db", index);
    let file_path = Path::new(DB_DIR).join("gi_partials/").join(&file_name);
    let result = scrape_game_infos(threads, &game_ids);
    let (games, _errors) = process_results(&result);
    let data = serde_json::to_string(&games).unwrap();
    let mut info_file = OpenOptions::new()
      .read(true)
      .write(true)
      .create(true)
      .open(&file_path)
      .expect(format!("Couldn't open/create file {}", &file_path.display()).as_ref());
    match info_file.write_all(data.as_bytes()) {
      Ok(_) => {
        println!(
          "Successfully wrote {} game infos to file {}. ({} bytes). Encountered {} errors",
          games.len(),
          &file_path.display(),
          data.len(),
          _errors.len()
        );
      }
      Err(e) => {
        println!(
          "Failed to write serialized data to {}. Error: {}",
          &file_path.display(),
          e
        )
      }
    }
  }
}

pub fn scrape_gameinfos(db_root_dir: &Path, game_ids: Vec<usize>) -> Vec<InternalGameInfo> {
  println!("Scraping remaining {} game info items", game_ids.len());
  let chunks: Vec<Vec<usize>> = game_ids
    .chunks(100)
    .map(|chunk| chunk.iter().map(|val| *val).collect())
    .collect();
  let mut scraped_missing: Vec<Vec<InternalGameInfo>> = Vec::new();

  let partials_dir = db_root_dir.join("gi_partials");
  let partials_file_count = std::fs::read_dir(&partials_dir)
    .expect("Could not open gi_partials directory")
    .count();
  let threads: usize = std::thread::available_parallelism()
    .expect("Failed to get cpus?")
    .try_into()
    .expect("Failed to convert");
  for (index, game_ids) in chunks.iter().enumerate() {
    let file_name = format!("gameinfo_partial-{}.db", index + partials_file_count);
    let file_path = &partials_dir.join(&file_name);
    let result = scrape_game_infos(threads, &game_ids);
    let (games, _errors) = process_results(&result);
    let data = serde_json::to_string(&games).unwrap();
    let mut info_file = OpenOptions::new()
      .read(true)
      .write(true)
      .create(true)
      .open(&file_path)
      .expect(format!("Couldn't open/create file {}", file_path.display()).as_ref());
    match info_file.write_all(data.as_bytes()) {
      Ok(_) => {
        println!(
          "Successfully wrote {} game infos to file {}. ({} bytes)",
          games.len(),
          file_path.display(),
          data.len()
        );
      }
      Err(e) => {
        println!(
          "Failed to write serialized data to {}. Error: {}",
          &file_path.display(),
          e
        );
      }
    }
    scraped_missing.push(games.into_iter().map(|x| x.clone()).collect::<Vec<InternalGameInfo>>());
  }
  scraped_missing.into_iter().flatten().collect()
}

pub fn handle_serde_json_error(err: serde_json::Error) {
  // do stuff with error
  println!("{}", err);
  panic!("De-serializing data failed. Make sure data is in the correct format, or delete all current data and re-scrape everything (Warning, may take a long time)");
}

fn main() {
  let db_root_dir = Path::new(DB_DIR);

  let args: Vec<String> = std::env::args().skip(1).collect();
  let hsviewer_directory_string = args
    .get(0)
    .expect("You need to provide directory of the HSViewer client application.");
  let hsviewer_binary_dir = Path::new(hsviewer_directory_string);
  if !hsviewer_binary_dir.exists() {
    panic!("Directory doesn't exist!")
  }
  println!(
    "You provided {} as directory containing hsviewer binary.",
    hsviewer_binary_dir.canonicalize().unwrap().display()
  );
  println!("This command needs to be run from within the client application folder.");
  match write_league_to_file(construct_league(), Path::new("./assets/db/teams.db")) {
    Ok(bytes_written) => {
      println!("Wrote teams to DB file, {} bytes", bytes_written);
    }
    Err(_write_error) => {}
  }

  let mut game_info_file = OpenOptions::new()
    .read(true)
    .write(true)
    .create(true)
    .open(&Path::new(DB_DIR).join("gameinfo.db"))
    .expect(
      format!(
        "Couldn't open/create file {}",
        &Path::new(DB_DIR).join("gameinfo.db").display()
      )
      .as_ref(),
    );

  let mut scraped_games =
    ScrapedSeason::new(&Path::new(DB_DIR).join("gameresults.db")).expect("Couldn't set up Scraped Season data");
  // let (mut db, bytes_read) = processing::process_game_info_db(&mut game_info_file);
  let game_info_db = processing::process_game_infos(&db_root_dir);
  match game_info_db {
    GameInfoScraped::None(None) => {
      game_info_scrape_all();
    }
    GameInfoScraped::None(Some(serde_err)) => {
      // An error occured while trying to de-serialize stored data. we just panic for now
      handle_serde_json_error(serde_err);
    }
    GameInfoScraped::Partial(partial_data, Some(missing_games)) => {
      let missing_scraped = scrape_gameinfos(db_root_dir, missing_games);
      let total: Vec<&InternalGameInfo> = partial_data.iter().chain(missing_scraped.iter()).collect();
      let de_duplicated: HashMap<usize, &InternalGameInfo> = total.iter().map(|val| (val.get_id(), *val)).collect();

      assert_eq!(de_duplicated.len(), total.len());
      if total.len() == GAMES_IN_SEASON {
        let data = serde_json::to_string(&total).expect("Could not de-serialize fully compiled Game Info db to file");
        match game_info_file.write_all(data.as_bytes()) {
          Ok(_) => println!("Successfully wrote serialized data of fully compiled Game Info db"),
          Err(e) => println!("Failed to write game info: {}", e),
        }
      } else {
        panic!("Still haven't scraped all Game Info objects. Run hockeyscraper again.");
      }
    }
    GameInfoScraped::Partial(all_data_as_partials, None) => {
      // Means we have all the data, it is just not compiled to a single file yet.
      let data = serde_json::to_string(&all_data_as_partials)
        .expect("Could not de-serialize fully compiled Game Info db to file");
      match game_info_file.write_all(data.as_bytes()) {
        Ok(_) => {
          println!("Successfully compiled partial data to single file & write to disk!");
        }
        Err(e) => {
          println!("Could not compile partial data and write to file: {}", e);
        }
      }
    }
    GameInfoScraped::All(season) => {
      // begin scraping & processing of Game Result data
      println!("All game infos are scraped & serialized to 1 file. Begin scraping of results...");
      let today = chrono::Utc::now();
      let date_tuple = (today.day(), today.month(), today.year() as u32);
      let refs = season
        .iter()
        .filter(|g| {
          g.is_played(date_tuple)
            && scraped_games
              .games
              .iter()
              .find(|game| game.id() == g.get_id())
              .is_none()
        })
        .map(|x| x)
        .collect();
      let (game_results, errors) = scrape_and_log(&refs);
      scraped_games.add_games(game_results);
      if !errors.is_empty() {
        println!("Error trying to scrape {} games", errors.len());
        for (id, err) in errors {
          println!("Game {} could not be scraped: {:?}", id, err);
        }
      }
      match scraped_games.serialize() {
        Ok(_) => println!("Done"),
        Err(err) => println!("Serialization failed: {}", err),
      }
    }
  }
}
