#include "gamestatistics.h"
GameStatistics::GameStatistics(int pk, int pp, int gf, int ga, int sf, int sa, int ppg, int pkg)  :
    m_PK(static_cast<double>(pk)), m_PP(static_cast<double>(pp)), m_GF(static_cast<double>(gf)),
    m_GA(static_cast<double>(ga)), m_SF(static_cast<double>(sf)), m_SA(static_cast<double>(sa)),
    m_PPG(static_cast<double>(ppg)), m_PKG(static_cast<double>(pkg))
{

}

GameStatistics::GameStatistics(double pk, double pp, double gf, double ga, double sf, double sa, double ppg, double pkg)  :
    m_PK(pk), m_PP(pp), m_GF(gf), m_GA(ga), m_SF(sf), m_SA(sa), m_PPG(ppg), m_PKG(pkg) {

}

GameStatistics GameStatistics::from(const std::string& team, const GameModel &gm)
{
    auto team_t = gm.get_team_type(team);
    auto pp = gm.pp_attempts(team_t);
    auto pk = gm.pk_attempts(team_t);
    auto gf = gm.goals_by(team_t);
    auto ga = gm.goals_by(gm.get_opponent_team_type(team));
    auto sf = gm.shots_by(team_t);
    auto sa = gm.shots_by(gm.get_opponent_team_type(team));
    auto ppg = gm.pp_goals(team_t);
    auto pkg = gm.pk_letups(team_t);
    return GameStatistics{pk, pp, gf, ga, sf, sa, ppg, pkg};
}
