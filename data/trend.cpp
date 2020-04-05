#include "trend.h"
#include <utils.h>
TrendComparisonAccumulator::TrendComparisonAccumulator(std::string team) :
    team(team), m_PK{0}, m_PP{0},
    m_GF{0},m_GA{0}, m_SF{0},
    m_SA{0}, m_PPG{0}, m_PKG{0}, games_analyzed(0)
{

}

double TrendComparisonAccumulator::gf_average() const {
    return divide_integers(m_GF, games_analyzed);
}

double TrendComparisonAccumulator::ga_average() const
{
    return divide_integers(m_GA, games_analyzed);
}

double TrendComparisonAccumulator::sf_average() const
{
    return divide_integers(m_SF, games_analyzed);
}

double TrendComparisonAccumulator::sa_average() const
{
    return divide_integers(m_SA, games_analyzed);
}

double TrendComparisonAccumulator::pp_efficiency() const
{
    return divide_integers(m_PPG, m_PP);
}

double TrendComparisonAccumulator::pk_efficiency() const
{
    return 1.0-(divide_integers(m_PKG,  m_PK));
}

double TrendComparisonAccumulator::pp_times_average() const
{
    return divide_integers(m_PP, games_analyzed);
}

double TrendComparisonAccumulator::pk_times_average() const
{
    return divide_integers(m_PK, games_analyzed);
}

void TrendComparisonAccumulator::push_game_stats(const GameStatistics &gs)
{
    this->m_PK  += gs.m_PK;
    this->m_PP  += gs.m_PP;
    this->m_GF  += gs.m_GF;
    this->m_GA  += gs.m_GA;
    this->m_SF  += gs.m_SF;
    this->m_SA  += gs.m_SA;
    this->m_PPG += gs.m_PPG;
    this->m_PKG += gs.m_PKG;
    games_analyzed++;
}

TrendComparison::TrendComparison(double pk, double pp, double gf, double ga, double sf, double sa, double ppt, double pkt) :
    m_PK(pk), m_PP(pp), m_GF(gf), m_GA(ga), m_SF(sf), m_SA(sa), m_PPT(ppt), m_PKT(pkt)
{

}

TrendComparison::TrendComparison(const TrendComparisonAccumulator &trendData, const GameStatistics &gs) :
    m_PK(gs.pk_efficiency() - trendData.pk_efficiency()),
    m_PP(gs.pp_efficiency() - trendData.pp_efficiency()),
    m_GF(gs.m_GF - trendData.gf_average()),
    m_GA(gs.m_GA - trendData.ga_average()),
    m_SF(gs.m_SF - trendData.sf_average()),
    m_SA(gs.m_SA - trendData.sa_average()),
    m_PPT(gs.m_PP - trendData.pp_times_average()),
    m_PKT(gs.m_PK - trendData.pk_times_average())
{

}

bool TrendComparison::ppIsBetter() const { return m_PP > 0.0; }         // Team increased amount of PK goals scored, compared to average
bool TrendComparison::pkIsBetter() const { return m_PK > 0.0; }         // Team decreased amount of PK goals let up, compared to average
bool TrendComparison::ppTimesIsBetter() const { return m_PPT > 0.0; }   // Team increased amount of times in PP
bool TrendComparison::pkTimesIsBetter() const { return m_PKT < 0.0; }   // Team decreased amount of times in PK
bool TrendComparison::gfIsBetter() const { return m_GF > 0.0; }         // Team increased goals scored, compared to average
bool TrendComparison::gaIsBetter() const { return m_GA < 0.0; }         // Team decreased goals let up, compared to average
bool TrendComparison::sfIsBetter() const { return m_SF > 0.0; }         // Team increased shots made, compared to average
bool TrendComparison::saIsBetter() const { return m_SA < 0.0; }         // Team decreased shots against, compared to average


TrendComparison operator-(const TrendComparison &lhs, const TrendComparison &rhs)
{
    return TrendComparison{lhs.m_PK - rhs.m_PK,
    lhs.m_PP - rhs.m_PP,
    lhs.m_GF - rhs.m_GF,
    lhs.m_GA - rhs.m_GA,
    lhs.m_SF - rhs.m_SF,
    lhs.m_SA - rhs.m_SA,
    lhs.m_PPT - rhs.m_PPT,
    lhs.m_PKT - rhs.m_PKT};
}
