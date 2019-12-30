#pragma once
#include "data/gamemodel.h"
#include "data/gameinfomodel.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <iostream>
#include <vector>
#include <tuple>
#include <chrono>

class MDbConnection
{
private:
    mongocxx::instance m_db_inst;
    mongocxx::client m_connection;
    std::string m_dbAddr;
    std::string m_dbName;
    bool is_connected;
    bool connect();
public:
    MDbConnection(const std::string& dbAddr, const std::string& m_dbName);
    MDbConnection(MDbConnection&& move);

    std::vector<GameModel> getGames(const std::string& team);
    std::vector<GameInfoModel> getTodaysGames();


    using DayRange = std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>;
    std::vector<GameInfoModel> getGamesAtDate(DayRange day);
private:

};
