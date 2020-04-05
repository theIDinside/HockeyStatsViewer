#pragma once

// System headers. These can be removed, as pre compiled headers are used in CMakeLists.txt
#include <string>
#include <ctime>
#include <chrono>
#include <optional>

// Other headers
#include <QString>
#include <valueholder.h>
#include "CalendarDate.h"

class GameInfoModel
{
public:
    int m_game_id;
private:
    TeamsValueHolder<std::string> m_teams;
public:
    CalendarDate m_date_played;
    GameInfoModel();
    GameInfoModel(int gameID, TeamsValueHolder<std::string> teams, std::chrono::system_clock::time_point date_time);
    GameInfoModel(int gameID, TeamsValueHolder<std::string> teams);
    GameInfoModel(const GameInfoModel& copy);
    GameInfoModel(GameInfoModel&& rhs) noexcept;
    GameInfoModel& operator=(const GameInfoModel& rhs);
    ~GameInfoModel() = default;

    [[nodiscard]] QString gameString() const;
    [[nodiscard]] std::string home_team() const;
    [[nodiscard]] std::string away_team() const;
    [[nodiscard]] std::string get_opponent_of(const std::string& team) const;
};
