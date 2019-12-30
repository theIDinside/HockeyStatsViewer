#pragma once
#include <string>
#include <ctime>
#include <chrono>
#include <QString>
#include <optional>
#include "valueholder.h"

class GameInfoModel
{
public:
    int m_game_id;
private:
    TeamsValueHolder<std::string> m_teams;
public:
    std::optional<std::chrono::system_clock::time_point> m_date_played;
    GameInfoModel();
    GameInfoModel(int gameID, TeamsValueHolder<std::string> teams, std::chrono::system_clock::time_point date_time);
    GameInfoModel(int gameID, TeamsValueHolder<std::string> teams);
    GameInfoModel(const GameInfoModel& copy);
    GameInfoModel(GameInfoModel&& rhs);
    GameInfoModel& operator=(const GameInfoModel& rhs);
    ~GameInfoModel() {}

    QString gameString() const;

    std::string home_team() const;
    std::string away_team() const;

    std::string get_opponent_of(const std::string& team) const;
};
