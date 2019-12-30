#pragma once
#include <string>
#include "gamemodel.h"

class GameModel;

struct GameStatistics {
    GameStatistics(int pk, int pp, int gf, int ga, int sf, int sa, int ppg, int pkg);
    GameStatistics(double pk, double pp, double gf, double ga, double sf, double sa, double ppg, double pkg);
    double m_PK;
    double m_PP;
    double m_GF;
    double m_GA;
    double m_SF;
    double m_SA;
    double m_PPG;
    double m_PKG;
    double pp_efficiency() const { return m_PPG / m_PP; }
    double pk_efficiency() const { return 1-(m_PKG / m_PK); }

    static GameStatistics from(const std::string& team, const GameModel& gm);
};
