#pragma once
#include "gamemodel.h"
#include "gamestatistics.h"

#include <string>
class GameStatistics;
class TrendComparisonAccumulator
{
public:
    explicit TrendComparisonAccumulator(std::string team);
    std::string team;
    int m_PK;
    int m_PP;
    int m_GF;
    int m_GA;
    int m_SF;
    int m_SA;
    int m_PPG;
    int m_PKG;

    double gf_average() const;
    double ga_average() const;
    double sf_average() const;
    double sa_average() const;
    double pp_efficiency() const;
    double pk_efficiency() const;
    double pp_times_average() const;
    double pk_times_average() const;

    void push_game_stats(const GameStatistics& gs);


    int games_analyzed;
};


struct TrendComparison {
    TrendComparison() = delete;
    TrendComparison(double pk, double pp, double gf, double ga, double sf, double sa, double ppt, double pkt);
    TrendComparison(const TrendComparisonAccumulator& acc, const GameStatistics& gs);
    double m_PK;
    double m_PP;
    double m_GF;
    double m_GA;
    double m_SF;
    double m_SA;
    double m_PPT;
    double m_PKT;

    friend TrendComparison operator-(const TrendComparison& lhs, const TrendComparison& rhs);

    bool ppIsBetter() const;
    bool pkIsBetter() const;
    bool ppTimesIsBetter() const;
    bool pkTimesIsBetter() const;
    bool gfIsBetter() const;
    bool gaIsBetter() const;
    bool sfIsBetter() const;
    bool saIsBetter() const;

};
