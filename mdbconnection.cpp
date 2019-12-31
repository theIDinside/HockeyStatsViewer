#include "mdbconnection.h"
#include "data/scoringmodel.h"

#include "utils.h"

#include <mongocxx/options/find.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <ctime>
#include <iomanip>
#include <cassert>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

MDbConnection::MDbConnection(const std::string& dbAddr, const std::string& dbName) : m_db_inst{}, m_connection{mongocxx::uri{}}, m_dbAddr{dbAddr}, m_dbName{dbName}
{

}

MDbConnection::MDbConnection(MDbConnection &&move) : m_db_inst(std::move(move.m_db_inst)), m_connection(std::move(move.m_connection)), m_dbAddr(std::move(move.m_dbAddr)), m_dbName(std::move(move.m_dbName)), is_connected(move.is_connected)
{

}

std::vector<GameModel> MDbConnection::getGames(const std::string &teamNameQuery)
{
    using bsoncxx::builder::stream::open_document;
    std::vector<GameModel> games{};
    auto collection = m_connection[m_dbName]["games"];

    auto cursor = collection.find(document{} << "$or" << open_array << open_document << "teams.home" << teamNameQuery << close_document << open_document << "teams.away" << teamNameQuery << close_document << close_array << finalize);
    int games_found = 0;
    for(auto& d : cursor) {
        auto awayTeam = d["teams"]["away"].get_utf8().value.to_string();
        auto homeTeam = d["teams"]["home"].get_utf8().value.to_string();
        auto gmTeams = TeamNames{homeTeam, awayTeam};
        std::vector<IntResults> shotsOnGoal{};
        auto periodShots = d["shotsOnGoal"].get_array().value;
        IntResults firstPeriod{periodShots.find(0)->get_document().value["home"].get_int32(), periodShots.find(0)->get_document().value["away"].get_int32()};
        IntResults secondPeriod{periodShots.find(1)->get_document().value["home"].get_int32(), periodShots.find(1)->get_document().value["away"].get_int32()};
        IntResults thirdPeriod{periodShots.find(2)->get_document().value["home"].get_int32(), periodShots.find(2)->get_document().value["away"].get_int32()};

        shotsOnGoal.push_back(firstPeriod);
        shotsOnGoal.push_back(secondPeriod);
        shotsOnGoal.push_back(thirdPeriod);
        auto shotsHome = std::accumulate(shotsOnGoal.begin(), shotsOnGoal.end(), 0, [&](auto& acc, const auto period) {
           return acc + period.home;
        });
        auto shotsAway = std::accumulate(shotsOnGoal.begin(), shotsOnGoal.end(), 0, [&](auto& acc, const auto period) {
            return acc + period.away;
        });
        assert(shotsHome < 100);
        assert(shotsAway < 100);
        std::cout << "Shots home/shots away" << shotsHome << "/" << shotsAway << std::endl;
        auto teamWon = d["teamWon"].get_utf8().value.to_string();
        IntResults finalResult{d["finalResult"]["home"].get_int32(), d["finalResult"]["away"].get_int32()};
        IntResults faceOffWins{d["faceOffWins"]["home"].get_int32(), d["faceOffWins"]["away"].get_int32()};
        PowerPlay pp{
            SpecialTeams{d["powerPlay"]["home"]["goals"].get_int32(), d["powerPlay"]["home"]["totals"].get_int32()},
            SpecialTeams{d["powerPlay"]["away"]["goals"].get_int32(), d["powerPlay"]["away"]["total"].get_int32()}
        };
        IntResults PIM{d["penaltyMinutes"]["home"].get_int32(), d["penaltyMinutes"]["away"].get_int32()};
        IntResults hits{d["hits"]["home"].get_int32(), d["hits"]["away"].get_int32()};
        IntResults blockedShots{d["blockedShots"]["home"].get_int32(), d["blockedShots"]["away"].get_int32()};
        IntResults giveAways{d["giveAways"]["home"].get_int32(), d["giveAways"]["away"].get_int32()};
        std::chrono::system_clock::time_point t{d["datePlayed"].get_date().value};

        auto scoringSummaryDoc = d["scoringSummary"].get_array().value;
        std::vector<ScoringModel> goals{};
        for(auto& goal : scoringSummaryDoc) {
            auto goalDoc = goal.get_document().view();
            auto firstAssist = goalDoc["assists"].get_array().value.find(0)->get_utf8().value.to_string();
            auto secondAssist = goalDoc["assists"].get_array().value.find(1)->get_utf8().value.to_string();

            auto [ass1, ass2] = convertAssistStrings(firstAssist, secondAssist);

            auto goalNumber = goalDoc["goal"].get_int32();
            auto period = goalDoc["period"].get_int32();
            auto timeDoc = goalDoc["time"].get_document().view();

            auto timeMins = timeDoc["minutes"].get_int32();
            auto timeSecs = timeDoc["seconds"].get_int32();

            auto strength = goalDoc["strength"].get_utf8().value.to_string();
            auto strength_t = ScoringModel::convert_strength_string(strength);
            auto scoringTeam = goalDoc["scoringTeam"].get_utf8().value.to_string();
            auto goalScorer = goalDoc["goalScorer"].get_utf8().value.to_string();
            GameTime gt{timeMins, timeSecs, period};
            auto fullTeamName = teams_map()[scoringTeam];
            ScoringModel sm{goalNumber, gt, strength_t, scoringTeam, goalScorer, ass1, ass2};
            goals.push_back(sm);
        }
        games.emplace_back(d["gameID"].get_int32().value, gmTeams, teamWon, t, shotsOnGoal, finalResult, faceOffWins, pp, PIM, hits, blockedShots, giveAways, std::move(goals));
        games_found++;
    }
    std::cout << "Games found: " << games_found << std::endl;
    return games;
}

std::vector<GameInfoModel> MDbConnection::getTodaysGames()
{
    using Teams = TeamsValueHolder<std::string>;
    auto [today, tomorrow] = make_date_range_from_today(1);
    // Construct date range in bson types
    bsoncxx::types::b_date dToday{today};
    bsoncxx::types::b_date dTomorrow{tomorrow};
    std::vector<GameInfoModel> games{};
    // query data base
    auto collection = m_connection[m_dbName]["gameinfos"];
    auto query = document{} << "datePlayed" << open_document << "$gte" << dToday << "$lt" << dTomorrow << close_document << finalize;
    auto cursor = collection.find(query.view());
    auto docs_found = 0;
    for(auto doc : cursor) {
          std::cout << bsoncxx::to_json(doc) << std::endl;
          docs_found++;

          int gameID = doc["gameID"].get_int32().value;
          Teams teams{doc["teams"]["home"].get_utf8().value.to_string(), doc["teams"]["away"].get_utf8().value.to_string()};
          std::chrono::system_clock::time_point datePlayed{doc["datePlayed"].get_date().value};
        games.emplace_back(gameID, teams, datePlayed);
    }
    return games;
}

GameModel MDbConnection::get_game(int gameID)
{
    auto collection = m_connection[m_dbName]["games"];
    auto query = document{} << "gameID" << gameID << finalize;
    auto cursor = collection.find(query.view());
    for(auto d : cursor) {
        auto awayTeam = d["teams"]["away"].get_utf8().value.to_string();
        auto homeTeam = d["teams"]["home"].get_utf8().value.to_string();
        auto gmTeams = TeamNames{homeTeam, awayTeam};
        std::vector<IntResults> shotsOnGoal{};
        auto periodShots = d["shotsOnGoal"].get_array().value;
        IntResults firstPeriod{periodShots.find(0)->get_document().value["home"].get_int32(), periodShots.find(0)->get_document().value["away"].get_int32()};
        IntResults secondPeriod{periodShots.find(1)->get_document().value["home"].get_int32(), periodShots.find(1)->get_document().value["away"].get_int32()};
        IntResults thirdPeriod{periodShots.find(2)->get_document().value["home"].get_int32(), periodShots.find(2)->get_document().value["away"].get_int32()};

        shotsOnGoal.push_back(firstPeriod);
        shotsOnGoal.push_back(secondPeriod);
        shotsOnGoal.push_back(thirdPeriod);
        auto shotsHome = std::accumulate(shotsOnGoal.begin(), shotsOnGoal.end(), 0, [&](auto& acc, const auto period) {
           return acc + period.home;
        });
        auto shotsAway = std::accumulate(shotsOnGoal.begin(), shotsOnGoal.end(), 0, [&](auto& acc, const auto period) {
            return acc + period.away;
        });
        std::cout << "Shots home/shots away" << shotsHome << "/" << shotsAway << std::endl;
        assert(shotsHome < 100);
        assert(shotsAway < 100);
        auto teamWon = d["teamWon"].get_utf8().value.to_string();
        IntResults finalResult{d["finalResult"]["home"].get_int32(), d["finalResult"]["away"].get_int32()};
        IntResults faceOffWins{d["faceOffWins"]["home"].get_int32(), d["faceOffWins"]["away"].get_int32()};
        PowerPlay pp{
            SpecialTeams{d["powerPlay"]["home"]["goals"].get_int32(), d["powerPlay"]["home"]["totals"].get_int32()},
            SpecialTeams{d["powerPlay"]["away"]["goals"].get_int32(), d["powerPlay"]["away"]["total"].get_int32()}
        };
        IntResults PIM{d["penaltyMinutes"]["home"].get_int32(), d["penaltyMinutes"]["away"].get_int32()};
        IntResults hits{d["hits"]["home"].get_int32(), d["hits"]["away"].get_int32()};
        IntResults blockedShots{d["blockedShots"]["home"].get_int32(), d["blockedShots"]["away"].get_int32()};
        IntResults giveAways{d["giveAways"]["home"].get_int32(), d["giveAways"]["away"].get_int32()};
        std::chrono::system_clock::time_point t{d["datePlayed"].get_date().value};

        auto scoringSummaryDoc = d["scoringSummary"].get_array().value;
        std::vector<ScoringModel> goals{};
        for(auto& goal : scoringSummaryDoc) {
            auto goalDoc = goal.get_document().view();
            auto firstAssist = goalDoc["assists"].get_array().value.find(0)->get_utf8().value.to_string();
            auto secondAssist = goalDoc["assists"].get_array().value.find(1)->get_utf8().value.to_string();

            auto [ass1, ass2] = convertAssistStrings(firstAssist, secondAssist);

            auto goalNumber = goalDoc["goal"].get_int32();
            auto period = goalDoc["period"].get_int32();
            auto timeDoc = goalDoc["time"].get_document().view();

            auto timeMins = timeDoc["minutes"].get_int32();
            auto timeSecs = timeDoc["seconds"].get_int32();

            auto strength = goalDoc["strength"].get_utf8().value.to_string();
            auto strength_t = ScoringModel::convert_strength_string(strength);
            auto scoringTeam = goalDoc["scoringTeam"].get_utf8().value.to_string();
            auto goalScorer = goalDoc["goalScorer"].get_utf8().value.to_string();
            GameTime gt{timeMins, timeSecs, period};
            auto fullTeamName = teams_map()[scoringTeam];
            ScoringModel sm{goalNumber, gt, strength_t, scoringTeam, goalScorer, ass1, ass2};
            goals.push_back(sm);
        }
        GameModel gm{d["gameID"].get_int32().value, gmTeams, teamWon, t, shotsOnGoal, finalResult, faceOffWins, pp, PIM, hits, blockedShots, giveAways, std::move(goals)};
        return gm;
    }
}

std::optional<GameInfoModel> MDbConnection::get_game_info(int gameID)
{
    using Teams = TeamsValueHolder<std::string>;
    // query data base
    auto collection = m_connection[m_dbName]["gameinfos"];
    auto query = document{} << "gameID" << gameID << finalize;
    auto cursor = collection.find(query.view());
    auto docs_found = 0;
    for(auto doc : cursor) {
          std::cout << bsoncxx::to_json(doc) << std::endl;
          docs_found++;

          int gameID = doc["gameID"].get_int32().value;
          Teams teams{doc["teams"]["home"].get_utf8().value.to_string(), doc["teams"]["away"].get_utf8().value.to_string()};
          std::chrono::system_clock::time_point datePlayed{doc["datePlayed"].get_date().value};
        return GameInfoModel{gameID, teams, datePlayed};
    }
    return {};
}

std::vector<GameInfoModel> MDbConnection::getGamesAtDate(DayRange day)
{
    using Teams = TeamsValueHolder<std::string>;
    auto& [begin, end] = day;
    bsoncxx::types::b_date dToday{begin};
    bsoncxx::types::b_date dTomorrow{end};
    std::vector<GameInfoModel> games{};
    // query data base
    auto collection = m_connection[m_dbName]["gameinfos"];
    auto query = document{} << "datePlayed" << open_document << "$gte" << dToday << "$lt" << dTomorrow << close_document << finalize;
    auto cursor = collection.find(query.view());
    auto docs_found = 0;
    for(auto doc : cursor) {
          std::cout << bsoncxx::to_json(doc) << std::endl;
          docs_found++;

          int gameID = doc["gameID"].get_int32().value;
          Teams teams{doc["teams"]["home"].get_utf8().value.to_string(), doc["teams"]["away"].get_utf8().value.to_string()};
          std::chrono::system_clock::time_point datePlayed{doc["datePlayed"].get_date().value};
        games.emplace_back(gameID, teams, datePlayed);
    }
    std::cout << "Found " << games.size() << " games" << std::  endl;
    return games;
}
