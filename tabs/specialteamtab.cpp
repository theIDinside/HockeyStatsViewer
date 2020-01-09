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

    chartPointers << pPowerPlay << pPenaltyKill << pPowerPlayPerGame << pPenaltyKillPerGame;

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
    auto PPHome = home.special_team_efficiency(Span::Five, ST::POWERPLAY);
    auto PPAway = away.special_team_efficiency(Span::Five, ST::POWERPLAY);
    auto PKHome= home.special_team_efficiency(Span::Five, ST::PENALTYKILL);
    auto PKAway = away.special_team_efficiency(Span::Five, ST::PENALTYKILL);

    auto homeTeam = home.team_name();
    auto awayTeam = away.team_name();
    std::vector<std::string> seriesNames{home.team_name(), home.team_name() + " Season", away.team_name(), away.team_name() + " Season"};

    auto PPHomeSeason = home.season_avg_last_x_games(5, [&](auto teamName, auto begin, auto end, auto divisor) -> double {
        auto [total_goals, total_att] = std::accumulate(begin, end, std::pair<int, int>{0, 0}, [&](auto& acc, const GameModel& game) {
            auto& [goals, attempts] = acc;
            auto [g, a]  = to_pair(game.get_special_teams(game.get_team_type(teamName), GameModel::SpecialTeamType::POWERPLAY));
            goals += g;
            attempts += a;
            return acc;
        });
        return static_cast<double>(total_goals) / static_cast<double>(total_att);
    });

    auto PPAwaySeason = away.season_avg_last_x_games(5, [&](auto teamName, auto begin, auto end, auto divisor) -> double {
        auto [total_goals, total_att] = std::accumulate(begin, end, std::pair<int, int>{0, 0}, [&](auto& acc, const GameModel& game) {
            auto& [goals, attempts] = acc;
            auto [g, a]  = to_pair(game.get_special_teams(game.get_team_type(teamName), GameModel::SpecialTeamType::POWERPLAY));
            goals += g;
            attempts += a;
            return acc;
        });
        return static_cast<double>(total_goals) / static_cast<double>(total_att);
    });

    auto PKHomeSeason = home.season_avg_last_x_games(5, [&](auto teamName, auto begin, auto end, auto divisor) -> double {
        auto [total_goals, total_att] = std::accumulate(begin, end, std::pair<int, int>{0, 0}, [&](auto& acc, const GameModel& game) {
            auto& [goals, attempts] = acc;
            auto [g, a]  = to_pair(game.get_special_teams(game.get_team_type(teamName), GameModel::SpecialTeamType::PENALTYKILL));
            goals += g;
            attempts += a;
            return acc;
        });
        return 1.0 - (static_cast<double>(total_goals) / static_cast<double>(total_att));
    });

    auto PKAwaySeason = away.season_avg_last_x_games(5, [&](auto teamName, auto begin, auto end, auto divisor) -> double {
        auto [total_goals, total_att] = std::accumulate(begin, end, std::pair<int, int>{0, 0}, [&](auto& acc, const GameModel& game) {
            auto& [goals, attempts] = acc;
            auto [g, a]  = to_pair(game.get_special_teams(game.get_team_type(teamName), GameModel::SpecialTeamType::PENALTYKILL));
            goals += g;
            attempts += a;
            return acc;
        });
        return 1.0 - (static_cast<double>(total_goals) / static_cast<double>(total_att));
    });



    auto pp_times_home = home.times_in_pp_game_average(Span::Five);
    auto pp_times_away = away.times_in_pp_game_average(Span::Five);
    auto pk_times_home = home.times_in_pk_game_average(Span::Five);
    auto pk_times_away = away.times_in_pk_game_average(Span::Five);

    auto format_to_percent = [&](auto& value) { return value*100.0; };
    std::transform(PPHome.begin(), PPHome.end(), PPHome.begin(), format_to_percent);
    std::transform(PPAway.begin(), PPAway.end(), PPAway.begin(), format_to_percent);
    std::transform(PPHomeSeason.begin(), PPHomeSeason.end(), PPHomeSeason.begin(), format_to_percent);
    std::transform(PPAwaySeason.begin(), PPAwaySeason.end(), PPAwaySeason.begin(), format_to_percent);

    std::transform(PKHome.begin(), PKHome.end(), PKHome.begin(), format_to_percent);
    std::transform(PKAway.begin(), PKAway.end(), PKAway.begin(), format_to_percent);
    std::transform(PKHomeSeason.begin(), PKHomeSeason.end(), PKHomeSeason.begin(), format_to_percent);
    std::transform(PKAwaySeason.begin(), PKAwaySeason.end(), PKAwaySeason.begin(), format_to_percent);

    std::vector<std::vector<double>> PPData{PPHome, PPHomeSeason, PPAway, PPAwaySeason};
    std::vector<std::vector<double>> PKData{PKHome, PKHomeSeason, PKAway, PKAwaySeason};

    pPowerPlay->clear_and_update_new_multi_series_data(seriesNames, PPData, RType::Percent);
    pPenaltyKill->clear_and_update_new_multi_series_data(seriesNames, PKData, RType::Percent);
    pPowerPlayPerGame->clear_and_update_new_data(home.team_name(), away.team_name(), pp_times_home, pp_times_away);
    pPenaltyKillPerGame->clear_and_update_new_data(home.team_name(), away.team_name(), pk_times_home, pk_times_away);
}

void SpecialTeamTab::set_chart_title_string_prefix(QString string)
{
    for(auto& chart : chartPointers)
        chart->set_title(QString("%1 %2").arg(chart->get_title()).arg(string));
}
