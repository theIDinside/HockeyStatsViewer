//
// Created by cx on 2020-03-21.
//

#pragma once

#include <filesystem>
#include <vector>
#include <map>
#include <db/types/GameResults.h>
#include <db/types/Team.h>
#include <data/gameinfomodel.h>

namespace fs = std::filesystem;

class DB {
public:

protected:
    DB(std::map<int, Team>&& teams, std::map<int, GameInfo>&& schedule, std::map<int, Game>&& games, std::map<CalendarDate, std::set<int>>&& calendar);
public:
    ~DB();
    static std::optional<std::unique_ptr<DB>> init_database(const std::string& game_info_path);
    std::optional<std::vector<GameInfoModel>> get_games_today();
    std::optional<std::vector<GameInfoModel>> get_games_at_date(const CalendarDate& date);
    std::optional<std::vector<GameInfoModel>> get_games_at_date(const QDate& date);
    std::optional<GameInfoModel> get_game_info(int game_id);


    std::vector<GameModel> get_games(const std::string& team);
    std::vector<GameModel> get_games_before(const std::string& team, int game_id);
    GameModel get_game(int game_id);

private:
    static std::vector<GameInfo> deserialize_game_infos(fs::path db_assets_dir);
    static std::vector<Game> deserialize_game_results(fs::path db_assets_dir);
    static std::vector<Team> deserialize_teams(fs::path db_assets_dir);

    std::map<int, Team> m_teams;
    std::map<int, GameInfo> m_schedule;
    std::map<int, Game> m_played_games;
    std::map<CalendarDate, std::set<int>> m_calendar; // utilized for quick search. This way we don't end up scanning the map/sets thousands of times per request
    std::map<Team, std::set<const Game*>> m_games_by_team; // this is safe. We only hold references to data we absolutely own.
};