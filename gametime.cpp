// System headers. These can be removed, as pre compiled headers are used in CMakeLists.txt
#include <cmath>

// Other headers
#include <QDebug>
#include <cassert>
#include "gametime.h"

/* Constructors */
GameTime::GameTime() : m_time{} , m_period(0) {

}

GameTime::GameTime(Time t, int period) : m_time(t), m_period(period) {

}

GameTime::GameTime(int mins, int secs, int period) : m_time{mins, secs}, m_period(period) {

}

GameTime::GameTime(const GameTime &gt) : m_time{gt.m_time}, m_period(gt.m_period) {

}

GameTime::GameTime(GameTime &&rval) : m_time(std::move(rval.m_time)), m_period(rval.m_period) {

}
/* /Constructors */

GameTime &GameTime::operator=(const GameTime &rhs)
{
    qDebug() << "Copy assignment constructor for Game Time called";
    if(this != &rhs) {
        m_time = rhs.m_time;
        m_period = rhs.m_period;
    }
    return *this;
}

bool GameTime::operator<(const GameTime &rhs) const
{
    return to_seconds() < rhs.to_seconds();
}

bool GameTime::operator>(const GameTime &rhs) const
{
    return to_seconds() > rhs.to_seconds();
}

bool GameTime::operator==(const GameTime &rhs) const
{
    return to_seconds() == rhs.to_seconds();
}

bool GameTime::operator<=(const GameTime &rhs) const
{
    return to_seconds() <= rhs.to_seconds();
}

bool GameTime::operator>=(const GameTime &rhs) const
{
    return to_seconds() >= rhs.to_seconds();
}

void GameTime::fast_forward(int minutes)
{
    if(m_time.m_minutes + minutes > 20 && m_time.m_seconds > 0) {
        auto minuteOverFlow = (m_time.m_minutes + minutes) - 20;
        if(m_period < 3) {
            m_period++;
        } else {
            throw std::runtime_error{"Minute overflow over 60 not allowed. Regular games are max 60 minutes long, then OT, which is not accounted for (yet)"};
        }
        m_time.m_minutes = minuteOverFlow;
    } else {
        m_time.m_minutes += minutes;
    }
}

Time GameTime::period_time_left()
{
    if(m_period < 4) {
        auto total_seconds_played = m_time.to_seconds();
        auto full_period = 20*60;
        return Time{full_period - total_seconds_played};
    } else if(m_period == 4) {
        auto total_seconds_played = m_time.to_seconds();
        auto full_period = 5*60;
        return Time{full_period - total_seconds_played};
    } else {
        return {}; // will return Time(0, 0);
    }
}

static constexpr auto full_overtime_secs = 5*60;
static constexpr auto full_regulation_game_seconds = 60*60;

Time GameTime::game_time_left()
{
    if(m_period < 4) {
        auto seconds_left = full_regulation_game_seconds - to_seconds();
        return Time{seconds_left};
    } else {
        auto seconds_played_overtime = m_time.to_seconds();
        return Time{full_overtime_secs-seconds_played_overtime};
    }
}


int GameTime::to_seconds() const
{
    auto seconds = m_time.to_seconds() + (m_period-1)*(20*60);
    return seconds;
}

std::string GameTime::debug() const
{
    return "P" + std::to_string(this->m_period) + " " +std::to_string(this->m_time.m_minutes) + ":" + std::to_string(this->m_time.m_seconds);
}

bool GameTime::in_range_of(const GameTime& begin, const GameTime& end) const {
    assert(begin < end);
    return begin <= *this && *this <= end;
}

GameTime from_seconds(int seconds)
{
    GameTime gt{0, 0, 0};
    auto minutes = seconds / 60; // integer division. Should always be floored by definition. Right?
    auto secs = seconds % 60;
    auto period = 1 + minutes / 20;
    auto periodMinutes = minutes % 20;
    return GameTime{periodMinutes, secs, period};
}

Time::Time(int seconds) : m_minutes(static_cast<int>(std::floor(static_cast<double>(seconds) / 60.0))), m_seconds(seconds % 60) { }
int Time::to_seconds() const{ return m_minutes*60 + m_seconds; }
