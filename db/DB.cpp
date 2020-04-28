//
// Created by cx on 2020-03-21.
//

#include "DB.h"

#include <utility>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

using json = nlohmann::json;


void to_json(json& j, const CalendarDate& d) {
    j = json{{"day", d.day}, {"month", d.month}, {"year", d.year }};
}
void from_json(const json& j, CalendarDate& d) {
    j.at("day").get_to(d.day);
    j.at("month").get_to(d.month);
    j.at("year").get_to(d.year);
}

void to_json(json& j, const GameInfo& gi) {
    j = json{
            { "home", gi.m_home }, {"away", gi.m_away},
            { "date", {"day", gi.m_date.day}, {"month", gi.m_date.month}, {"year", gi.m_date.year }},
            { "gid", gi.m_game_id }
    };
}

void from_json(const json& j, GameInfo& gi) {
    j.at("home").get_to(gi.m_home);
    j.at("away").get_to(gi.m_away);
    j.at("date").get_to(gi.m_date);
    j.at("gid").get_to(gi.m_game_id);
}

void to_json(json& j, const Goal& goal) {
    j = json {
        {"goal_number", goal.m_goal_number},
        {"player", goal.m_goal_scorer},
        {"team", goal.m_scoring_team},
        {"period", 
            {"number", goal.m_time.m_period}, 
            {"time", 
                {"minutes", goal.m_time.m_time.m_minutes}, 
                {"seconds", goal.m_time.m_time.m_seconds}
            }
        },
    };
}

void from_json(const json& j, Goal& goal) {
    // TODO: This is not fully implemented and will create runtime logical errors (strength not read for example)
    j.at("goal_number").get_to(goal.m_goal_number);
    j.at("player").get_to(goal.m_goal_scorer);
    j.at("team").get_to(goal.m_scoring_team);
    j.at("period").at("number").get_to(goal.m_time.m_period);
    j.at("period").at("time").at("minutes").get_to(goal.m_time.m_time.m_minutes);
    j.at("period").at("time").at("seconds").get_to(goal.m_time.m_time.m_seconds);

    std::string strength = j["strength"];
    if(strength == "Even") {
        goal.m_strength = TeamStrength::TS_EVEN;
    } else if(strength == "PowerPlay") {
        goal.m_strength = TeamStrength::TS_POWER_PLAY;
    } else if(strength == "EvenEmptyNet") {
        goal.m_strength = TeamStrength::TS_EVEN_EMPTY_NET;
    } else if(strength == "Shootout") {
        goal.m_strength = TeamStrength::TS_SHOOTOUT;
    } else if(strength == "ShortHanded") {
        goal.m_strength = TeamStrength::TS_SHORT_HANDED;
    } else if(strength == "PowerPlayPenaltyShot") {
        goal.m_strength = TeamStrength::TS_POWER_PLAY_PENALTY_SHOT;
    } else if(strength == "ShortHandedEmptyNet") {
        goal.m_strength = TeamStrength::TS_SHORT_HANDED_EMPTY_NET;
    } else if(strength == "ShortHandedPenaltyShot") {
        goal.m_strength = TeamStrength::TS_SHORT_HANDED_PENALTY_SHOT;
    } else if(strength == "PowerPlayEmptyNet") {
        goal.m_strength = TeamStrength::TS_POWER_PLAY_EMPTY_NET;
    } else if(strength == "EvenPenaltyShot") {
        goal.m_strength = TeamStrength::TS_EVEN_PENALTY_SHOT;
    } else if(strength == "PenaltyShot") {
        goal.m_strength = TeamStrength::TS_PENALTY_SHOT;
    } else {
        std::cout << "failed reading strength for goal. Exiting" << std::endl;
        std::abort();
    }
}

void from_json(const json& j, IntResults& value_pair) {
    j.at("home").get_to(value_pair.home);
    j.at("away").get_to(value_pair.away);
}


void to_json(json& j, const Game& g) {
    j = json{ {"game_info", g.m_game_info }, {"foo", 3} };
}

void from_json(const json& j, Game& game) {
    j.at("game_info").get_to(game.m_game_info);
    j.at("goals").get_to(game.m_goals);
    j.at("winning_team").get_to(game.m_winning_team);
    j.at("final_score").at("away").get_to(game.m_final_result.away);
    j.at("final_score").at("home").get_to(game.m_final_result.home);
    j.at("shots").get_to(game.m_shots);
    j.at("power_plays").at("away").at("goals").get_to(game.m_power_play.away.m_goals);
    j.at("power_plays").at("away").at("total").get_to(game.m_power_play.away.m_attempts);
    j.at("power_plays").at("home").at("goals").get_to(game.m_power_play.home.m_goals);
    j.at("power_plays").at("home").at("total").get_to(game.m_power_play.home.m_attempts);
    j.at("take_aways").get_to(game.m_take_aways);
    j.at("give_aways").get_to(game.m_give_aways);
    j.at("face_offs").at("away").get_to(game.m_face_off.away);
    j.at("face_offs").at("home").get_to(game.m_face_off.home);
}

DB::DB(std::map<int, Team>&& teams, std::map<int, GameInfo>&& schedule, std::map<int, Game>&& games, std::map<CalendarDate, std::set<int>>&& calendar) :
    m_teams{std::move(teams)}, m_schedule{std::move(schedule)}, 
    m_played_games{std::move(games)}, m_calendar{std::move(calendar)},
    m_games_by_team{}
{    

    for(const auto& [id, t] : m_teams) {
        m_games_by_team.emplace(t, std::set<const Game*>{});
    }

    // Not a code smell. We are creating a map of references to data we own. Must therefore be done here.
    for(const auto& [game_id, game_result] : m_played_games) {
        const auto& home_team = game_result.m_game_info.m_home;
        const auto& away_team = game_result.m_game_info.m_away;        
        auto home_search = std::find_if(m_teams.cbegin(), m_teams.cend(), [&](const auto& iter) {
            const auto& team = iter.second;
            if(team == home_team) {
                return true;
            }
            return false;
        });
        auto away_search = std::find_if(m_teams.cbegin(), m_teams.cend(), [&](const auto& iter) { 
            const auto& team = iter.second;
            if(team == away_team) {
                return true;
            }
            return false;            
        });

        if(home_search != m_teams.end() && away_search != m_teams.end()) {
            auto ht = home_search->second;
            auto at = away_search->second;

            m_games_by_team[ht].insert(&game_result);
            m_games_by_team[at].insert(&game_result);
        } else {
            std::cout << "failed to initialize DB." << std::endl;
            std::abort();
        }
    }
    std::cout << "DB initialized. Teams and their game count:\n";
    for(const auto& [team, played_games] : m_games_by_team) {
        std::cout << team.team_name() << " played " << played_games.size() << " games.\n";
    }
    std::cout << std::endl;
}

DB::~DB() {

}

std::vector<GameInfo> DB::deserialize_game_infos(fs::path db_assets_dir) {
    auto game_infos = db_assets_dir / "gameinfo.db";
    if(!fs::exists(game_infos)) {
        std::cout << "COULD NOT FIND / FILE DOES NOT EXIST: " << game_infos << std::endl;
        std::abort();
    }
    std::ifstream f{game_infos};
    std::stringstream ss{};
    ss << f.rdbuf();
    auto data = ss.str();
    std::vector<GameInfo> games = json::parse(data);
    return games;
}
std::vector<Game> DB::deserialize_game_results(fs::path db_assets_dir) {
    // read file gameresult.db to string, then parse it using nlohmann json lib
    auto game_results = db_assets_dir / "gameresults.db";
    if(!fs::exists(game_results)) {
        std::cout << "COULD NOT FIND / FILE DOES NOT EXIST: " << game_results << std::endl;
        std::abort();
    }
    std::ifstream f{game_results};
    std::stringstream ss{};
    ss << f.rdbuf();
    auto data = ss.str();
    std::vector<Game> games = json::parse(data);
    return games;
}
std::vector<Team> DB::deserialize_teams(fs::path db_assets_dir) {
    // read file teams.db to string, then parse it using nlohmann json lib

    auto teams_file = db_assets_dir / "teams.db";
    if(!fs::exists(teams_file)) {
        std::cout << "COULD NOT FIND / FILE DOES NOT EXIST: " << teams_file << std::endl;
        std::abort();
    }
    std::ifstream f{teams_file};
    std::stringstream ss{};
    ss << f.rdbuf();
    auto data = ss.str();
    std::vector<Team> teams = json::parse(data);
    return teams;
}


std::optional<std::unique_ptr<DB>> DB::init_database(const std::string &game_info_path) {
    fs::path db_asset_path{game_info_path};
    if(fs::exists(db_asset_path)) {
        auto teams_data = DB::deserialize_teams(db_asset_path);
        auto game_infos_data = DB::deserialize_game_infos(db_asset_path);
        auto game_results_data = DB::deserialize_game_results(db_asset_path);

        std::map<int, Team> teams{};
        std::map<int, GameInfo> gameInfos{};
        std::map<int, Game> gameResults{};
        std::map<Team, std::set<Game*>> games_by_team; // this is safe. We only hold references to data we absolutely own.

        for(const auto& t : teams_data) {
            teams.emplace(t.id(), t);
        }

        for(const auto& gi : game_infos_data) {
            gameInfos.emplace(gi.m_game_id, gi);
        }

        for(const auto& gr: game_results_data) {
            gameResults.emplace(gr.m_game_info.m_game_id, gr);
        }
        std::map<CalendarDate, std::set<int>> calendar{};
        for(const auto& [id, game_info] : gameInfos) {
            auto it = std::find_if(calendar.begin(), calendar.end(), [date=game_info.m_date](const auto& it) {
                return it.first == date;
            });
            if(it != calendar.end()) {
                calendar[game_info.m_date].insert(game_info.m_game_id);
            } else {
                std::set<int> new_set{};
                new_set.insert(game_info.m_game_id);
                calendar.emplace(game_info.m_date, std::move(new_set));
            }
        }
        std::unique_ptr<DB> db{new DB{std::move(teams), std::move(gameInfos), std::move(gameResults), std::move(calendar)}};
        return db;

    } else {
        std::cout << "ERROR: Directory for DB assets does not exist!" << db_asset_path << std::endl;
    }
    return {};
}


std::vector<GameModel> DB::get_games(const std::string& team_name) {
    std::vector<GameModel> games{};
    auto team_search = std::find_if(m_teams.cbegin(), m_teams.cend(), [&](const auto& iter) { 
        const auto& team = iter.second;
        if(team == team_name) {
            return true;
        }
        return false;            
    });
    if(team_search != m_teams.end()) {
        auto team = team_search->second;
        auto played_games = m_games_by_team[team];
        for(const auto game_pointer: played_games) {
            games.push_back(Game::convert(*game_pointer));
        }
        return games;
    }
    return games;
}
std::vector<GameModel> DB::get_games_before(const std::string& team, int game_id) {
    return {};
}
GameModel DB::get_game(int game_id) {
    auto g = m_played_games.at(game_id);
    return Game::convert(g);
}

std::optional<std::vector<GameInfoModel>> DB::get_games_today() {
    return {};
}

std::optional<std::vector<GameInfoModel>> DB::get_games_at_date(const CalendarDate& date) {

    if(m_calendar.find(date) != m_calendar.end()) {
        auto game_ids = m_calendar[date];
        std::vector<GameInfoModel> games;

        std::transform(game_ids.cbegin(), game_ids.cend(), std::back_inserter(games), [&](const auto& id) {
            return GameInfo::convert(m_schedule[id]);
        });

        return games;
    }
    return {};
}

std::optional<std::vector<GameInfoModel>> DB::get_games_at_date(const QDate& date) {
    return get_games_at_date(CalendarDate::from(date));
}

std::optional<GameInfoModel> DB::get_game_info(int game_id) {
    if(m_schedule.find(game_id) != m_schedule.end()) {
        auto game_info = m_schedule[game_id];
        return GameInfoModel{game_info.m_game_id, TeamNames{game_info.m_home, game_info.m_away}, game_info.m_date};
    } else {
        return {};
    }
}