use super::data::gameinfo::InternalGameInfo;

use crate::GAMES_IN_SEASON;

pub type FileResult = Result<String, std::io::Error>;

pub trait FileString {
  fn string_read(&mut self) -> FileResult;
}

pub fn check_missing(games: &Vec<InternalGameInfo>) -> Option<Vec<usize>> {
  let mut missing_games = Vec::new();
  let ids: Vec<usize> = games.iter().map(|x| x.get_id()).collect();
  for game in crate::FULLSEASON {
    if !ids.contains(&game) {
      missing_games.push(game);
    }
  }

  if missing_games.len() == 0 {
    None
  } else {
    Some(missing_games)
  }
}

pub fn verify_deserialized_content(data: &Vec<InternalGameInfo>) -> bool {
  data.len() == GAMES_IN_SEASON
}
