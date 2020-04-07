#pragma once
#include <memory>

template <typename T>
struct TeamsValueHolder {
    TeamsValueHolder() : home{}, away{} {}
    TeamsValueHolder(const T& home, const T& away) : home(home), away(away) {}
    TeamsValueHolder(const TeamsValueHolder& t) : home(t.home), away(t.away) {}
    TeamsValueHolder(TeamsValueHolder&& t) : home(std::move(t.home)), away(std::move(t.away)) {}
    ~TeamsValueHolder() {}
    TeamsValueHolder& operator=(const TeamsValueHolder& rhs) {
        if(this != &rhs) {
            this->home = rhs.home;
            this->away = rhs.away;
        }
        return *this;
    }

    TeamsValueHolder& operator=(TeamsValueHolder&& rhs) {
        if(this != &rhs) {
            this->home = std::move(rhs.home);
            this->away = std::move(rhs.away);
        }
        return *this;
    }

    T home;
    T away;
};



struct SpecialTeams {
    enum SPType {
        PowerPlay,
        PenaltyKilling
    };
    SpecialTeams() : m_goals(0), m_attempts(0) {}
    SpecialTeams(int goals, int attempts) : m_goals(goals), m_attempts(attempts) {}
    SpecialTeams(const SpecialTeams& spt) : m_goals{spt.m_goals}, m_attempts{spt.m_attempts} {}
    SpecialTeams(SpecialTeams&& m) : m_goals(m.m_goals), m_attempts(m.m_attempts) {}
    SpecialTeams& operator=(const SpecialTeams& rhs) {
        if(this != &rhs) {
            this->m_goals = rhs.m_goals;
            this->m_attempts = rhs.m_attempts;
        }
        return *this;
    }
    ~SpecialTeams() {}
    int m_goals;
    int m_attempts;

    friend std::pair<int, int> to_pair(const SpecialTeams& data) {
            return std::make_pair(data.m_goals, data.m_attempts);
    }

    double get_efficiency(SPType type) const {
        if(m_attempts == 0) return 0;
        double result = -1;
        switch(type) {
            case SPType::PowerPlay:
                result = (static_cast<double>(m_goals) / static_cast<double>(m_attempts)) * 100;
            break;
            case SPType::PenaltyKilling:
                result = (1.0 - (static_cast<double>(m_goals) / static_cast<double>(m_attempts))) * 100;
            break;
        }
        return result;
    }
};
