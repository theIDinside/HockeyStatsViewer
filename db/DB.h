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

private:
    DB(std::map<int, Team>&& teams, std::map<int, GameInfo>&& schedule, std::map<int, Game>&& games);
public:
    ~DB();
    static std::optional<std::unique_ptr<DB>> init_database(const std::string& game_info_path);
    std::optional<std::vector<GameInfoModel>> get_games_today();
    std::optional<std::vector<GameInfoModel>> get_games_at_date(const CalendarDate& date);
    std::optional<GameInfoModel> get_game_info(int game_id);


    std::vector<GameModel> get_games(const std::string& team);
    std::vector<GameModel> get_games_before(const std::string& team, int game_id);


private:
    bool deserialize_game_infos();
    bool deserialize_game_results();
    bool deserialize_teams();

    std::map<int, Team> m_teams;
    std::map<int, GameInfo> m_schedule;
    std::map<int, Game> m_played_games;


    std::map<CalendarDate, std::set<int>> m_calendar;

};