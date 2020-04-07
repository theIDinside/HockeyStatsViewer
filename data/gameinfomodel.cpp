#include "gameinfomodel.h"

GameInfoModel::GameInfoModel() : m_game_id{-1}, m_teams{"", ""},  m_date_played{}
{

}

GameInfoModel::GameInfoModel(int gameID, TeamsValueHolder<std::string> teams, CalendarDate date) : m_game_id{gameID}, m_teams{teams}, m_date_played{date}
{

}

GameInfoModel::GameInfoModel(int gameID, TeamsValueHolder<std::string> teams) : m_game_id{gameID}, m_teams{teams}, m_date_played{}
{

}

GameInfoModel::GameInfoModel(const GameInfoModel &copy) : m_game_id{copy.m_game_id}, m_teams{copy.m_teams}, m_date_played{copy.m_date_played}
{

}

GameInfoModel::GameInfoModel(GameInfoModel &&rhs) noexcept : m_game_id{rhs.m_game_id}, m_teams{std::move(rhs.m_teams)}, m_date_played{rhs.m_date_played}
{

}

GameInfoModel& GameInfoModel::operator=(const GameInfoModel& rhs) {
    if(this != &rhs) {
        this->m_game_id = rhs.m_game_id;
        this->m_teams = rhs.m_teams;
        this->m_date_played = rhs.m_date_played;
    }
    return *this;
}

QString GameInfoModel::gameString() const
{
    return QString{"%1 vs %2"}.arg(m_teams.away.c_str(), m_teams.home.c_str());
}

std::string GameInfoModel::home_team() const
{
    return m_teams.home;
}

std::string GameInfoModel::away_team() const
{
    return m_teams.away;
}

std::string GameInfoModel::get_opponent_of(const std::string &team) const
{
    if(team == m_teams.home) {
        return m_teams.away;
    } else if(team == m_teams.away) {
        return m_teams.home;
    } else {
        auto gamestring = gameString();
        auto err_mess = QString("Error, team provided %1, did not play in this game! Teams playing in this game: %2").arg(team.c_str()).arg(gamestring);
        throw std::runtime_error(err_mess.toStdString());
    }
}
