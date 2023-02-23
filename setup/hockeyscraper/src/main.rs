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

use std::fs::OpenOptions;
use std::io::{Read, Write};
use std::path::{Path, PathBuf};
use std::time::Instant;

use data::{
  game::Game,
  gameinfo::InternalGameInfo,
  team::{construct_league, write_league_to_file},
};
use scrape::errors::BuilderError;
use scrape::{process_results, scrape_game_infos};

use crate::scrape::GameInfoScraper;

const BASE_URL: &'static str = "https://www.nhl.com/gamecenter";

const FIRST_GAME: usize = 2022020001;
const GAMES_IN_SEASON: usize = 1312;
const DB_DIR: &'static str = "assets/db";

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

pub fn game_info_scrape_all() -> Vec<InternalGameInfo> {
  // Begin scraping of all game info
  let full_season_ids: Vec<usize> = (FIRST_GAME..(FIRST_GAME + GAMES_IN_SEASON)).collect();
  scrape_gameinfos(&Path::new(DB_DIR), full_season_ids)
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
  println!("Attempting to use {} threads...", threads);
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
    scraped_missing.push(
      games
        .into_iter()
        .map(|x| x.clone())
        .collect::<Vec<InternalGameInfo>>(),
    );
  }
  scraped_missing.into_iter().flatten().collect()
}

pub fn handle_serde_json_error(err: serde_json::Error) {
  // do stuff with error
  println!("{}", err);
  panic!("De-serializing data failed. Make sure data is in the correct format, or delete all current data and re-scrape everything (Warning, may take a long time)");
}

fn main() {
  let args: Vec<String> = std::env::args().skip(1).collect();
  let hsviewer_directory_string = args
    .get(0)
    .expect("You need to provide directory of the HSViewer client application.");
  println!("provided path: {}", hsviewer_directory_string);
  let provided_path = Path::new(".").canonicalize().unwrap();
  let db_path = provided_path
    .canonicalize()
    .unwrap_or(PathBuf::from("."))
    .join(DB_DIR);

  if !provided_path.exists() {
    panic!(
      "Directory doesn't exist! {}",
      provided_path
        .canonicalize()
        .unwrap_or(PathBuf::new())
        .display()
    );
  }
  println!(
    "You provided {} as directory containing hsviewer binary.",
    provided_path.canonicalize().unwrap().display()
  );

  match write_league_to_file(construct_league(), &db_path.join("teams.db")) {
    Ok(bytes_written) => {
      println!("Wrote teams to DB file, {} bytes", bytes_written);
    }
    Err(_write_error) => {}
  }
  println!("db path: {}", db_path.display());
  let games = GameInfoScraper::new(&db_path)
    .scrape_game_infos()
    .scrape_games();
  println!("{}", games);
  games.serialize();
}
