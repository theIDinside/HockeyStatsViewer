//
// Created by cx on 2020-03-21.
//

#include "DB.h"

#include <utility>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>


std::unique_ptr<DB> DB::make_db(const std::string &game_info_path, const std::string &game_results_path) {
    fs::path GIPath{game_info_path};
    fs::path GRPath{game_results_path};

    if( !(fs::exists(GIPath) && fs::exists(GRPath)) ) {
        throw std::runtime_error{"Paths containing data for database does not exist. Run scrape in binary folder."};
    }


    std::ifstream GIFile{GIPath};
    std::string string_data;
    GIFile >> string_data;

    auto gi_json_data = json::parse(string_data);



    DB db{GIPath, GRPath};



}

DB::DB(std::map<int, Team>&& teams, std::map<int, GameInfo>&& schedule, std::map<int, Game>&& games) :
    m_teams{std::move(teams)}, m_schedule{std::move(schedule)}, m_played_games{std::move(games)}, m_calendar{}
{

}

DB::~DB() {

}

bool DB::deserialize_teams() {

    return false;
}

std::optional<std::unique_ptr<DB>> DB::init_database(const std::string &game_info_path) {
    fs::path db_asset_path{game_info_path};
    if(fs::exists(db_asset_path)) {

    }

    return {}
}
