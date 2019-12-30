#include <charts/linechart.h>
#include "specialteamtab.h"

#include <QLineSeries>
#include <QValueAxis>

SpecialTeamTab::SpecialTeamTab(QWidget* parent) : TeamStatsTab(parent), m_layout{this}
{
    LineChart* pp = new LineChart("Power play");
    LineChart* pk = new LineChart("Penalty kill");
    LineChart* pp_pga = new LineChart("Times in power play");
    LineChart* pk_pga = new LineChart("Times in penalty kill");

    pPowerPlay = pp;
    pPenaltyKill = pk;
    pPowerPlayPerGame = pp_pga;
    pPenaltyKillPerGame = pk_pga;

    m_layout.addWidget(pp,0,0);
    m_layout.addWidget(pk, 0, 1);
    m_layout.addWidget(pp_pga, 1, 0);
    m_layout.addWidget(pk_pga, 1, 1);

    this->setLayout(&m_layout);
}

void SpecialTeamTab::update_chart_data(const TeamStats &home, const TeamStats &away)
{
    using Span = TeamStats::Span;
    using ST = GameModel::SpecialTeamType;
    using RType = LineChartRangeType;
    auto pp_home_efficiency = home.special_team_efficiency(Span::Five, ST::POWERPLAY);
    auto pp_away_efficiency = away.special_team_efficiency(Span::Five, ST::POWERPLAY);
    auto pk_home_efficiency = home.special_team_efficiency(Span::Five, ST::PENALTYKILL);
    auto pk_away_efficiency = away.special_team_efficiency(Span::Five, ST::PENALTYKILL);
    auto pp_times_home = home.times_in_pp_game_average(Span::Five);
    auto pp_times_away = away.times_in_pp_game_average(Span::Five);
    auto pk_times_home = home.times_in_pk_game_average(Span::Five);
    auto pk_times_away = away.times_in_pk_game_average(Span::Five);

    auto format_from_percent = [&](auto& value) { return value*100.0; };
    std::transform(pp_home_efficiency.begin(), pp_home_efficiency.end(), pp_home_efficiency.begin(), format_from_percent);
    std::transform(pp_away_efficiency.begin(), pp_away_efficiency.end(), pp_away_efficiency.begin(), format_from_percent);
    std::transform(pk_home_efficiency.begin(), pk_home_efficiency.end(), pk_home_efficiency.begin(), format_from_percent);
    std::transform(pk_away_efficiency.begin(), pk_away_efficiency.end(), pk_away_efficiency.begin(), format_from_percent);

    pPowerPlay->clear_and_update_new_data(home.team_name(), away.team_name(), pp_home_efficiency, pp_away_efficiency, RType::Percent);
    pPenaltyKill->clear_and_update_new_data(home.team_name(), away.team_name(), pk_home_efficiency, pk_away_efficiency, RType::Percent);
    pPowerPlayPerGame->clear_and_update_new_data(home.team_name(), away.team_name(), pp_times_home, pp_times_away);
    pPenaltyKillPerGame->clear_and_update_new_data(home.team_name(), away.team_name(), pk_times_home, pk_times_away);
}
