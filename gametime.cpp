#include "gametime.h"
#include <cmath>

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

Time::Time(int seconds) : m_minutes(static_cast<int>(std::floor(static_cast<double>(seconds) / 60.0))), m_seconds(seconds % 60) { }
int Time::to_seconds() const{ return m_minutes*60 + m_seconds; }
