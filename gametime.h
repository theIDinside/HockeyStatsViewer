#pragma once
#include <tuple>

struct Time {

    Time() : m_minutes(0), m_seconds(0) {}
    explicit Time(int seconds);
    Time(int mins, int secs) : m_minutes(mins), m_seconds(secs) {}
    Time(const Time& t) : m_minutes(t.m_minutes), m_seconds(t.m_seconds) {}
    Time(Time&& t) : m_minutes(t.m_minutes), m_seconds(t.m_seconds) {}

    ~Time() = default;
    bool operator==(const Time& t)  { return to_seconds() == t.to_seconds();          }
    bool operator<(const Time& t)   { return to_seconds() < t.to_seconds();     }
    bool operator>(const Time& t)   { return to_seconds() > t.to_seconds();     }
    Time& operator=(const Time& rhs) {
        if(this != &rhs) {
            m_minutes = rhs.m_minutes;
            m_seconds = rhs.m_seconds;
        }
        return *this;
    }

    Time& operator=(Time&& rhs) noexcept {
        if(this != &rhs) {
            m_minutes = rhs.m_minutes;
            m_seconds = rhs.m_seconds;
        }
        return *this;
    }

    int to_seconds() const;

    int m_minutes;
    int m_seconds;
};

class GameTime
{
public:
    Time m_time;
    int m_period;
    GameTime();
    GameTime(Time t, int period);
    GameTime(int mins, int secs, int period);
    GameTime(const GameTime& gt);
    GameTime(GameTime&& rval);
    ~GameTime() = default;

    GameTime& operator=(const GameTime& rhs);

    bool operator<(const GameTime& rhs) const;
    bool operator>(const GameTime& rhs) const;
    bool operator==(const GameTime& rhs) const;
    bool operator<=(const GameTime& rhs) const;
    bool operator>=(const GameTime& rhs) const;

    /**
     * @brief time_left_period
     * @return Time
     */
    Time period_time_left();
    Time game_time_left();
    int to_seconds() const;
};
