#pragma once
#include <gametime.h>

enum At {
    HOME,
    AWAY
};

class StandingAnalysis {
    StandingAnalysis(const std::string& team, int home, int away, At at) : team(team), home(home), away(away), at(at) {

    }

    int difference() const {
        if(at == HOME) {
            return home - away;
        } else {
            return away - home;
        }
    }
    std::string team;
    int home, away;
    At at;
};

class Standing
{
public:
    Standing(std::string home_team, std::string away_team, int home, int away, GameTime gametime);
    Standing(Standing& copy) = default;

    int away_difference();
    int home_difference();
    int difference_of(const std::string& team);
    const std::string home;
    const std::string away;
    int mHome;
    int mAway;
    GameTime mGameTime;

    friend std::ostream& operator<<(std::ostream& os, const Standing& s) {
        // HV71 - LHC: 1-3, 19:39 P2
        std::string output{s.away + " vs " + s.home +": " + s.score_to_string() +", P" + std::to_string(s.mGameTime.m_period) + " " + std::to_string(s.mGameTime.m_time.m_minutes)+":"+std::to_string(s.mGameTime.m_time.m_seconds)};
        os << output;
        return os;
    }

    std::string score_to_string() const {
        return std::to_string(mAway) + "-" + std::to_string(mHome);
    }

};
