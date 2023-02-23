use serde::ser::SerializeStruct;
use serde::{Serialize, Serializer};
use std::convert::TryFrom;

#[derive(Copy, Clone, Debug, Serialize, Deserialize)]
pub struct Time {
  minutes: u16,
  seconds: u16,
}

impl Time {
  pub fn new(minutes: u16, seconds: u16) -> Time {
    Time { minutes, seconds }
  }
}

#[derive(Copy, Clone, Debug, Deserialize)]
pub enum Period {
  First(Time),
  Second(Time),
  Third(Time),
  OT(Time),
  SO,
}

#[derive(Copy, Clone, Debug, Deserialize, Serialize)]
pub struct DeserializePeriod {
  number: usize,
  time: Time,
}

impl Serialize for Period {
  fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
  where
    S: Serializer,
  {
    let mut state = serializer.serialize_struct("Period", 2)?;
    match self {
      Period::First(t) => {
        state.serialize_field("number", &1)?;
        state.serialize_field("time", &t)?;
      }
      Period::Second(t) => {
        state.serialize_field("number", &2)?;
        state.serialize_field("time", &t)?;
      }
      Period::Third(t) => {
        state.serialize_field("number", &3)?;
        state.serialize_field("time", &t)?;
      }
      Period::OT(t) => {
        state.serialize_field("number", &4)?;
        state.serialize_field("time", &t)?;
      }
      Period::SO => {
        let t = Time::new(0, 0); // Shootout does not have time. So we set it to an "invalid" time, 0:00
        state.serialize_field("number", &5)?;
        state.serialize_field("time", &t)?;
      }
    }
    state.end()
  }
}

impl Period {
  pub fn new(period_string: &str, time: Option<Time>) -> Option<Period> {
    match period_string {
      "1" => Some(Period::First(time.unwrap())),
      "2" => Some(Period::Second(time.unwrap())),
      "3" => Some(Period::Third(time.unwrap())),
      "OT" => Some(Period::OT(time.unwrap())),
      "SO" => Some(Period::SO),
      _ => None,
    }
  }
}

#[derive(Copy, Clone, Debug, Deserialize)]
pub enum GoalStrength {
  Even = 0,
  EvenPenaltyShot = 1,
  PenaltyShot = 2,
  EvenEmptyNet = 3,
  PowerPlay = 4,
  ShortHanded = 5,
  ShortHandedEmptyNet = 6,
  ShortHandedPenaltyShot = 7,
  PowerPlayEmptyNet = 8,
  PowerPlayPenaltyShot = 9,
  Shootout = 10,
}

impl Serialize for GoalStrength {
  fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
  where
    S: Serializer,
  {
    match self {
      GoalStrength::Even => serializer.serialize_u8(0),
      GoalStrength::EvenPenaltyShot => serializer.serialize_u8(1),
      GoalStrength::PenaltyShot => serializer.serialize_u8(2),
      GoalStrength::EvenEmptyNet => serializer.serialize_u8(3),
      GoalStrength::PowerPlay => serializer.serialize_u8(4),
      GoalStrength::ShortHanded => serializer.serialize_u8(5),
      GoalStrength::ShortHandedEmptyNet => serializer.serialize_u8(6),
      GoalStrength::ShortHandedPenaltyShot => serializer.serialize_u8(7),
      GoalStrength::PowerPlayEmptyNet => serializer.serialize_u8(8),
      GoalStrength::PowerPlayPenaltyShot => serializer.serialize_u8(9),
      GoalStrength::Shootout => serializer.serialize_u8(10),
    }
  }
}

impl TryFrom<&String> for GoalStrength {
  type Error = &'static str;
  fn try_from(s: &String) -> Result<Self, Self::Error> {
    type GS = GoalStrength;
    match s.as_ref() {
      "EV" => Ok(GS::Even),
      "EV-EN" => Ok(GS::EvenEmptyNet),
      "PP" => Ok(GS::PowerPlay),
      "EV-PS" => Ok(GS::EvenPenaltyShot),
      "PS" => Ok(GS::PenaltyShot),
      "SH" => Ok(GS::ShortHanded),
      "SH-PS" => Ok(GS::ShortHandedPenaltyShot),
      "SH-EN" => Ok(GS::ShortHandedEmptyNet),
      "PP-EN" => Ok(GS::PowerPlayEmptyNet),
      "PP-PS" => Ok(GS::PowerPlayPenaltyShot),
      "SO" => Ok(GS::Shootout),
      _ => Err("Could not parse string into GoalStrength type"),
    }
  }
}

#[derive(Copy, Clone, Debug, Serialize, Deserialize)]
pub struct FaceOffs {
  pub away: f32,
  pub home: f32,
}

#[derive(Copy, Clone, Debug, Serialize, Deserialize)]
pub struct PowerPlay {
  /// Power play goals made in this game
  pub goals: u32,
  /// Power play attempts in this game
  pub total: u32,
}

#[derive(Copy, Clone, Debug, Serialize, Deserialize)]
pub struct PowerPlays {
  pub away: PowerPlay,
  pub home: PowerPlay,
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct Player {
  // player name
  name: String,
  // player id is: (team id, jersey)
  id: (u8, u8),
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct Goal {
  /// Player name
  player: Player,
  /// Scoring team
  pub team: usize,
  /// Period & time in period
  period: Period,
  /// Team strengths
  strength: GoalStrength,
  /// Assists
  assists: Vec<Player>,
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct DeserializeGoal {
  /// Player name
  player: Player,
  /// Scoring team
  pub team: usize,
  period: DeserializePeriod,
  /// Team strengths
  strength: GoalStrength,
  /// Assists
  assists: Vec<Player>,
}

impl From<DeserializeGoal> for Goal {
  fn from(g: DeserializeGoal) -> Goal {
    Goal {
      player: g.player,
      team: g.team,
      period: Period::from(g.period),
      strength: g.strength,
      assists: g.assists,
    }
  }
}

impl From<DeserializePeriod> for Period {
  fn from(p: DeserializePeriod) -> Period {
    if p.number == 1 {
      Period::First(p.time)
    } else if p.number == 2 {
      Period::Second(p.time)
    } else if p.number == 3 {
      Period::Third(p.time)
    } else if p.number == 4 {
      Period::OT(p.time)
    } else if p.number == 5 {
      Period::SO
    } else {
      panic!("Period number is erroneous.");
    }
  }
}

impl Goal {
  pub fn new_as_opt(
    player: Player,
    team: usize,
    period: Period,
    strength: GoalStrength,
    assists: Vec<Player>,
  ) -> Option<Goal> {
    Some(Goal {
      player,
      team,
      period,
      strength,
      assists,
    })
  }

  pub fn by_team(&self, team: usize) -> bool {
    self.team == team
  }
}

#[derive(Debug)]
pub struct GoalBuilder {
  player: Option<Player>,
  team: Option<usize>,
  period: Option<Period>,
  strength: Option<GoalStrength>,
  assists: Vec<Player>,
  pub was_ps_shot: bool,
}

impl GoalBuilder {
  pub fn new() -> GoalBuilder {
    GoalBuilder {
      player: None,
      team: None,
      period: None,
      strength: None,
      assists: vec![],
      was_ps_shot: false,
    }
  }

  pub fn was_unsuccessful_penalty_shot(&mut self) {
    self.was_ps_shot = true;
  }

  fn make_player(team: usize, player_string: &String) -> Option<Player> {
    if player_string.len() < 3 || player_string == "unassisted" {
      None
    } else {
      let jersey_end = player_string
        .find(' ')
        .expect("Could not find name / jersey separator");
      let name_end = player_string.find('(').unwrap_or(player_string.len());
      let num = player_string[0..jersey_end]
        .parse::<u8>()
        .expect("failed to parse jersey number");
      Some(Player {
        name: player_string[jersey_end + 1..name_end].into(),
        id: (team as u8, num),
      })
    }
  }

  #[inline(always)]
  fn was_ps(&self) -> bool {
    match &self.strength.unwrap() {
      GoalStrength::EvenPenaltyShot
      | GoalStrength::PenaltyShot
      | GoalStrength::ShortHandedPenaltyShot
      | GoalStrength::PowerPlayPenaltyShot
      | GoalStrength::Shootout => true,
      _ => false,
    }
  }

  pub fn assist(&mut self, player: String) {
    if !self.was_ps() {
      if let Some(p) = GoalBuilder::make_player(self.team.unwrap(), &player) {
        self.assists.push(p);
      }
    }
  }

  pub fn player(&mut self, player: String) {
    self.player = GoalBuilder::make_player(self.team.unwrap(), &player);
  }
  pub fn team(&mut self, team: usize) {
    self.team = Some(team);
  }
  pub fn period(&mut self, period: Period) {
    self.period = Some(period);
  }
  pub fn strength(&mut self, strength: GoalStrength) {
    self.strength = Some(strength);
  }

  pub fn finalize(self) -> Option<Goal> {
    if let (Some(player), Some(team_id), Some(period), Some(strength)) =
      (self.player.clone(), self.team, self.period, self.strength)
    {
      Goal::new_as_opt(player, team_id, period, strength, self.assists)
    } else {
      None
    }
  }

  pub fn is_shootout(&self) -> bool {
    if let Some(period) = self.period {
      match period {
        Period::SO => true,
        _ => false,
      }
    } else {
      false
    }
  }
}

#[derive(Copy, Clone, Debug, Serialize, Deserialize)]
pub struct PIM {
  pub away: usize,
  pub home: usize,
}
#[derive(Copy, Clone, Debug, Serialize, Deserialize)]
pub struct Score {
  pub away: usize,
  pub home: usize,
}
#[derive(Copy, Clone, Debug, Serialize, Deserialize)]
pub struct Shots {
  pub away: usize,
  pub home: usize,
}
#[derive(Copy, Clone, Debug, Serialize, Deserialize)]
pub struct TakeAways {
  pub away: usize,
  pub home: usize,
}
#[derive(Copy, Clone, Debug, Serialize, Deserialize)]
pub struct GiveAways {
  pub away: usize,
  pub home: usize,
}

#[cfg(test)]
mod tests {
  use crate::data::stats::{Period, Time};

  #[test]
  fn test_serialization_of_period_first() {
    let should_be = r#"{"number":1,"time":{"minutes":18,"seconds":22}}"#;
    let period = Period::First(Time::new(18, 22));
    let period_serialized = serde_json::to_string(&period).unwrap();
    assert_eq!(should_be, period_serialized);
  }
  #[test]
  fn test_serialization_of_period_second() {
    let should_be = r#"{"number":2,"time":{"minutes":8,"seconds":2}}"#;
    let period = Period::Second(Time::new(8, 2));
    let period_serialized = serde_json::to_string(&period).unwrap();
    assert_eq!(should_be, period_serialized);
  }
  #[test]
  fn test_serialization_of_period_third() {
    let should_be = r#"{"number":3,"time":{"minutes":3,"seconds":49}}"#;
    let period = Period::Third(Time::new(3, 49));
    let period_serialized = serde_json::to_string(&period).unwrap();
    assert_eq!(should_be, period_serialized);
  }
  #[test]
  fn test_serialization_of_period_ot() {
    let should_be = r#"{"number":4,"time":{"minutes":4,"seconds":21}}"#;
    let period = Period::OT(Time::new(4, 21));
    let period_serialized = serde_json::to_string(&period).unwrap();
    assert_eq!(should_be, period_serialized);
  }

  #[test]
  fn test_serialization_of_period_shootout() {
    let should_be = r#"{"number":5,"time":{"minutes":0,"seconds":0}}"#;
    let period = Period::SO;
    let period_serialized = serde_json::to_string(&period).unwrap();
    assert_eq!(should_be, period_serialized);
  }
}
