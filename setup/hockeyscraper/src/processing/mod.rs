use std::collections::HashMap;
use super::data::gameinfo::InternalGameInfo;
use std::io::{Read};
use std::fs::OpenOptions;


pub enum GameInfoScraped {
    All(Vec<InternalGameInfo>),
    Partial(Vec<InternalGameInfo>, Vec<usize>),
    None(Option<serde_json::Error>)
}

impl From<serde_json::Error> for GameInfoScraped {
    fn from(e: serde_json::Error) -> GameInfoScraped {
        GameInfoScraped::None(Some(e))
    }
}

fn check_missing(games: &Vec<InternalGameInfo>) -> Option<Vec<usize>> {
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

pub fn process_game_infos(db_dir: &std::path::Path) -> GameInfoScraped {
    assert_eq!(db_dir.exists(), true);
    let compiled_file_name = "gameinfo.db";
    let partials_dir = db_dir.join("gi_partials");
    
    let f = db_dir.join(compiled_file_name);
    if partials_dir.exists() { // means we haven't compiled a full DB yet.
        println!("\nGame Info partials directory exists... Scanning contents");

        let mut all_partials = Vec::new();
        let mut count = 0;
        for entry in std::fs::read_dir(&partials_dir).expect(format!("Couldn't read directory {} or it's contents", partials_dir.display()).as_ref()) {
            let file_entry = entry.expect("Couldn't unwrap iterator for file");
            let mut buf = String::new();
            let mut f_handle = OpenOptions::new()
                                            .read(true)
                                            .write(false)
                                            .create(false)
                                            .open(file_entry.path())
                                            .expect(format!("Couldn't open file {} for processing", file_entry.path().display()).as_ref());                                            
            let bytes_read = f_handle.read_to_string(&mut buf)
                .expect(format!("Couldn't read file contents of {} to char buffer for processing", file_entry.path().display()).as_ref());
            if bytes_read <= 2 { // means we have EOF or just empty [] json array or empty json object {}
                // skip this file
            } else {
                let partials: Vec<InternalGameInfo> = serde_json::from_str(&buf)
                        .expect(format!("Couldn't de-serialize contents from file {}", file_entry.path().display()).as_ref());
                all_partials.push(partials);
            }
            count += 1;
        }
        if count == 0 {
            return GameInfoScraped::None(None);
        }
        let flattened: Vec<InternalGameInfo> = all_partials.into_iter().flatten().collect();
        let de_duped: HashMap<usize, &InternalGameInfo> = flattened.iter().map(|gi| (gi.get_id(), gi)).collect();
        assert_eq!(flattened.len(), de_duped.len());
        if let Some(missing) = check_missing(&flattened) {
            GameInfoScraped::Partial(flattened, missing)
        } else {
            let mut full = flattened.clone();
            full.sort_by(|a,b| a.get_id().partial_cmp(&b.get_id()).unwrap());
            GameInfoScraped::All(full)
        }
    } else {
        let game_info_full = db_dir.join(compiled_file_name);
        if game_info_full.exists() {
            let mut buf = String::new();
            let mut f_handle = 
                OpenOptions::new().read(true).write(false).create(false).open(&game_info_full)
                    .expect(format!("Couldn't open file {} for processing", game_info_full.display()).as_ref());
            let _bytes_read = f_handle.read_to_string(&mut buf).expect(format!("Couldn't read contents of {}", game_info_full.display()).as_ref());
            let data: Vec<InternalGameInfo> = serde_json::from_str(&buf).expect("Couldn't de-serialize data from Game Info DB file");
            let de_duped_data: HashMap<usize, &InternalGameInfo> = data.iter().map(|gi| (gi.get_id(), gi)).collect();
            assert_eq!(data.len(), de_duped_data.len());

            if let Some(missing_games) = check_missing(&data) {
                GameInfoScraped::Partial(data, missing_games)
            } else {
                let mut full = data.clone();
                full.sort_by(|a,b| a.get_id().partial_cmp(&b.get_id()).unwrap());
                GameInfoScraped::All(full)
            }
        } else {
            GameInfoScraped::None(None)        
        }
    }
}
