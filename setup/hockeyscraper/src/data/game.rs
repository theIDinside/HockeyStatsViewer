use super::gameinfo::InternalGameInfo;
use super::stats::{DeserializeGoal, FaceOffs, GiveAways, Goal, PowerPlays, Score, Shots, TakeAways};
use crate::data::stats::PowerPlay;
use crate::scrape::errors::BuilderError;
use std::io::{Read, Write};
use std::path::Path;
/// S.E = self explanatory

/// This is a real fine hack by me. I haven't set out to learn how to implement a custom Deserialize (yet), so for now I deserialize to this,
/// then convert to struct Game from that. The problem lies with the goals field, as the final Game struct has Goals represented with
/// the period field as a Period enum. And custom deserializing of enums is a bit tricky, so for now this will do, as the main purpose
/// of this part of the application is the serialization part. The custom serialization for Period is implemented in stats.rs,
/// the rest is derived using #[derive(Deserialize)]
#[derive(Debug, Serialize, Deserialize)]
pub struct IntermediateGame {
  /// Holds game ID, home team ID, away team ID and CalendarDate for when it was played
  game_info: InternalGameInfo,
  /// Scoring progression
  goals: Vec<DeserializeGoal>,
  /// Winning team's ID
  winning_team: String,
  /// S.E.
  final_score: Score,
  /// Shots by period
  shots: Vec<Shots>,
  /// Power play results for teams in this game
  power_plays: PowerPlays,
  /// S.E.
  take_aways: TakeAways,
  /// S.E.
  give_aways: GiveAways,
  /// Face off win percentages
  face_offs: FaceOffs,
}

#[derive(Debug, Serialize, Deserialize, Clone)]
pub struct Game {
  /// Holds game ID, home team ID, away team ID and CalendarDate for when it was played
  game_info: InternalGameInfo,
  /// Scoring progression
  goals: Vec<Goal>,
  /// Winning team's ID
  winning_team: String,
  /// S.E.
  final_score: Score,
  /// Shots by period
  shots: Vec<Shots>,
  /// Power play results for teams in this game
  power_plays: PowerPlays,
  /// S.E.
  take_aways: TakeAways,
  /// S.E.
  give_aways: GiveAways,
  /// Face off win percentages
  face_offs: FaceOffs,
}

impl Game {
  pub fn id(&self) -> usize {
    self.game_info.get_id()
  }
}

pub struct ScrapedSeason {
  pub games: Vec<Game>,
  file: std::path::PathBuf,
}

impl ScrapedSeason {
  pub fn new(file: &Path) -> Result<ScrapedSeason, std::io::Error> {
    let mut game_results = std::fs::OpenOptions::new()
      .read(true)
      .write(true)
      .create(true)
      .open(file)
      .expect(format!("Couldn't open/create file {}", file.display()).as_ref());
    let mut buf = String::new();
    match game_results.read_to_string(&mut buf) {
      Ok(bytes) if bytes < 2 => Ok(ScrapedSeason {
        games: vec![],
        file: file.to_path_buf(),
      }),
      Ok(bytes) => {
        #[cfg(debug_assertions)]
        println!("Read {} bytes", bytes);
        let data: Vec<IntermediateGame> =
          serde_json::from_str(&buf).expect("Couldn't de-serialize data for Game results");
        #[cfg(debug_assertions)]
        println!("Deserialized {} games", data.len());
        let games: Vec<Game> = data.into_iter().map(|im_game| Game::from(im_game)).collect();
        Ok(ScrapedSeason {
          games,
          file: file.to_path_buf(),
        })
      }
      Err(e) => Err(e),
    }
  }

  pub fn add_games(&mut self, games: Vec<Game>) {
    self.games.extend(games);
  }

  pub fn serialize(&mut self) -> Result<(), std::io::Error> {
    let mut game_results = std::fs::OpenOptions::new()
      .read(true)
      .write(true)
      .create(true)
      .truncate(true)
      .open(&self.file)
      .expect(format!("Couldn't open/create file {}", &self.file.display()).as_ref());
    let data = serde_json::to_string(&self.games).expect("Couldn't serialize game results data");
    match game_results.write_all(data.as_bytes()) {
      Ok(_) => {
        println!("Successfully wrote serialized data to file");
        Ok(())
      }
      Err(e) => {
        println!("Could not write serialized data to file");
        Err(e)
      }
    }
  }
}

impl From<IntermediateGame> for Game {
  fn from(g: IntermediateGame) -> Game {
    Game {
      game_info: g.game_info,
      goals: g.goals.into_iter().map(|g| Goal::from(g)).collect(),
      winning_team: g.winning_team,
      final_score: g.final_score,
      shots: g.shots,
      power_plays: g.power_plays,
      take_aways: g.take_aways,
      give_aways: g.give_aways,
      face_offs: g.face_offs,
    }
  }
}

#[allow(dead_code)]
impl Game {
  /// This struct can be optimized. But there's really no point, as it *will* be serialized and not
  /// used in the front-end which is entirely written in C++
  pub fn size_of(&self) -> usize {
    std::mem::size_of::<InternalGameInfo>()
      + self.goals.len() * std::mem::size_of::<Goal>()
      + self.winning_team.len() * std::mem::size_of::<char>()
      + std::mem::size_of::<Score>()
      + self.shots.len() * std::mem::size_of::<Shots>()
      + std::mem::size_of::<PowerPlay>() * 2
      + std::mem::size_of::<TakeAways>()
      + std::mem::size_of::<GiveAways>()
      + std::mem::size_of::<FaceOffs>()
  }
}

#[derive(Clone, Debug)]
pub struct ValueHolder<T> {
  away: Option<T>,
  home: Option<T>,
}

impl ValueHolder<PowerPlay> {
  pub fn as_power_plays(&self) -> Option<PowerPlays> {
    if let (Some(away), Some(home)) = (self.away, self.home) {
      Some(PowerPlays { away, home })
    } else {
      None
    }
  }
}

impl ValueHolder<usize> {
  pub fn as_take_aways(&self) -> Option<TakeAways> {
    if let (Some(away), Some(home)) = (self.away, self.home) {
      Some(TakeAways { away, home })
    } else {
      None
    }
  }

  pub fn as_give_aways(&self) -> Option<GiveAways> {
    if let (Some(away), Some(home)) = (self.away, self.home) {
      Some(GiveAways { away, home })
    } else {
      None
    }
  }

  pub fn as_final_score(&self) -> Option<Score> {
    if let (Some(away), Some(home)) = (self.away, self.home) {
      Some(Score { away, home })
    } else {
      None
    }
  }
}

impl ValueHolder<f32> {
  pub fn as_face_offs(&self) -> Option<FaceOffs> {
    if let (Some(away), Some(home)) = (self.away, self.home) {
      Some(FaceOffs { away, home })
    } else {
      None
    }
  }
}

#[derive(Debug)]
pub enum TeamValue<T> {
  Away(T),
  Home(T),
}

#[derive(Debug)]
pub struct GameBuilder {
  game_info: Option<InternalGameInfo>,
  goals: Option<Vec<Goal>>,
  winning_team: Option<usize>,
  final_score: ValueHolder<usize>,
  shots: Option<Vec<Shots>>,
  power_plays: ValueHolder<PowerPlay>,
  take_aways: ValueHolder<usize>,
  give_aways: ValueHolder<usize>,
  face_offs: ValueHolder<f32>,
}

impl GameBuilder {
  pub fn new() -> GameBuilder {
    GameBuilder {
      game_info: None,
      goals: None,
      winning_team: None,
      final_score: ValueHolder { away: None, home: None },
      shots: None,
      power_plays: ValueHolder { away: None, home: None },
      take_aways: ValueHolder { away: None, home: None },
      give_aways: ValueHolder { away: None, home: None },
      face_offs: ValueHolder { away: None, home: None },
    }
  }

  pub fn game_info(&mut self, game_info: InternalGameInfo) {
    self.game_info = Some(game_info);
  }

  pub fn add_goal(&mut self, goal: Goal) {
    if let Some(ref mut goals) = self.goals {
      goals.push(goal);
    } else {
      let mut goals = Vec::new();
      goals.push(goal);
      self.goals = Some(goals);
    }
  }

  pub fn shots(&mut self, shots: Vec<Shots>) {
    self.shots = Some(shots);
  }

  pub fn power_plays(&mut self, power_plays: TeamValue<PowerPlay>) {
    match power_plays {
      TeamValue::Away(away) => self.power_plays.away = Some(away),
      TeamValue::Home(home) => self.power_plays.home = Some(home),
    }
  }

  pub fn take_aways(&mut self, take_aways: TeamValue<usize>) {
    match take_aways {
      TeamValue::Away(away) => self.take_aways.away = Some(away),
      TeamValue::Home(home) => self.take_aways.home = Some(home),
    }
  }

  pub fn give_aways(&mut self, give_aways: TeamValue<usize>) {
    match give_aways {
      TeamValue::Away(away) => self.give_aways.away = Some(away),
      TeamValue::Home(home) => self.give_aways.home = Some(home),
    }
  }

  pub fn face_offs(&mut self, face_offs: TeamValue<f32>) {
    match face_offs {
      TeamValue::Away(away) => self.face_offs.away = Some(away),
      TeamValue::Home(home) => self.face_offs.home = Some(home),
    }
  }

  pub fn set_final_score(&mut self) {
    let home_id = self.game_info.as_ref().unwrap().get_home_team();
    let away_id = self.game_info.as_ref().unwrap().get_away_team();

    let mut home = 0usize;
    let mut away = 0usize;

    if self.goals.is_none() {
      println!("Game ID: {:?}", self.game_info.as_ref().unwrap());
      println!("Game ID: {:?}", &self);
    }

    for goal in self.goals.as_ref().unwrap() {
      if goal.by_team(home_id) {
        home += 1;
      } else if goal.by_team(away_id) {
        away += 1;
      } else {
        let errmsg = format!("This goal is registered by a team not playing in this game. This is a forced panic (logic error). Away: {} - Home {}. Goal by {}", away_id, home_id, goal.team);
        panic!("{}", errmsg);
      }
    }
    self.final_score = ValueHolder {
      away: Some(away),
      home: Some(home),
    }
  }

  pub fn finalize(&self) -> Option<Game> {
    let fscore = self.final_score.as_final_score().unwrap().clone();

    let winning_team = if fscore.home > fscore.away {
      self.game_info.as_ref().unwrap().get_home_team().clone()
    } else {
      self.game_info.as_ref().unwrap().get_away_team().clone()
    };

    if let (
      Some(game_info),
      Some(goals),
      Some(final_score),
      Some(shots),
      Some(power_plays),
      Some(take_aways),
      Some(give_aways),
      Some(face_offs),
    ) = (
      *&self.game_info.as_ref(),
      &self.goals,
      *&self.final_score.as_final_score(),
      &self.shots,
      *&self.power_plays.as_power_plays(),
      *&self.take_aways.as_take_aways(),
      *&self.give_aways.as_give_aways(),
      *&self.face_offs.as_face_offs(),
    ) {
      Some(Game {
        game_info: game_info.clone(),
        goals: goals.clone(),
        winning_team,
        final_score,
        shots: shots.clone(),
        power_plays,
        take_aways,
        give_aways,
        face_offs,
      })
    } else {
      None
    }
  }

  pub fn get_error(&self) -> BuilderError {
    let mut err_fields = Vec::new();
    if self.game_info.is_none() {
      err_fields.push(format!("self.game_info not set"));
    }
    if self.goals.is_none() {
      err_fields.push("self.goals not set".to_owned());
    }
    if self.winning_team.is_none() {
      err_fields.push(String::from("self.winning_team not set"));
    }
    if self.final_score.away.is_none() || self.final_score.home.is_none() {
      err_fields.push(format!("self.final_score incorrect: {:?}", self.final_score));
    }
    if self.shots.is_none() {
      err_fields.push(String::from("self.shots not set"));
    }
    if self.power_plays.home.is_none() || self.power_plays.away.is_none() {
      err_fields.push(format!("self.power_plays incorrect: {:?}", self.power_plays));
    }
    if self.take_aways.home.is_none() || self.take_aways.away.is_none() {
      err_fields.push(format!("self.take_aways incorrect: {:?}", self.take_aways));
    }
    if self.give_aways.home.is_none() || self.give_aways.away.is_none() {
      err_fields.push(format!("self.give_aways incorrect: {:?}", self.give_aways));
    }
    if self.face_offs.home.is_none() || self.face_offs.away.is_none() {
      err_fields.push(format!("self.face_offs incorrect: {:?}", self.face_offs));
    }
    BuilderError::GameIncomplete(self.game_info.as_ref().unwrap().get_id(), err_fields)
  }
}

#[cfg(test)]
mod tests {}
