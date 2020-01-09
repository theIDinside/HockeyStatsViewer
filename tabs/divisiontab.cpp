#include "divisiontab.h"

DivisionTab::DivisionTab(QWidget *parent) : TeamStatsTab(parent), m_layout{this} {
    pPowerPlay      = new LineChart("Power Play");
    pPenaltyKill    = new LineChart("Penalty Kill");
    pTimesInPP      = new LineChart("Times in PP");
    pTimesInPK      = new LineChart("Times in PK");
    pGoalsFor       = new LineChart("Goals For");
    pGoalsAgainst   = new LineChart("Goals Against");
    pTotalGoals     = new LineChart("Total Goals");
    pShotEfficiency = new LineChart("Shot efficiency");

    chartPointers << pPowerPlay << pPenaltyKill << pTimesInPP << pTimesInPK << pGoalsFor << pGoalsAgainst << pTotalGoals << pShotEfficiency;

    for(auto& ptr : chartPointers) ptr->setMinimumHeight(350);
    m_layout.addWidget(pPowerPlay, 0, 0);
    m_layout.addWidget(pPenaltyKill, 0, 1);
    m_layout.addWidget(pTimesInPP, 1, 0);
    m_layout.addWidget(pTimesInPK, 1, 1);
    m_layout.addWidget(pGoalsFor, 2, 0);
    m_layout.addWidget(pGoalsAgainst, 2, 1);
    m_layout.addWidget(pTotalGoals, 3, 0);

}

struct IntermediateResult {
    IntermediateResult(int powerPlayGoals, int powerPlayAttempts, int penaltyKillDefended, int penaltyKillAttempts, int goalsFor, int goalsAgainst) :
        powerPlayGoals(powerPlayGoals), powerPlayAttempts(powerPlayAttempts),
        penaltyKillDefended(penaltyKillDefended), penaltyKillAttempts(penaltyKillAttempts),
        goalsFor(goalsFor), goalsAgainst(goalsAgainst)
    {

    }

    int powerPlayGoals;
    int powerPlayAttempts;
    int penaltyKillDefended;
    int penaltyKillAttempts;
    int goalsFor;
    int goalsAgainst;
};

void DivisionTab::update_chart_data(const TeamStats &home, const TeamStats &away)
{

    auto homeTeam = home.team_name();
    auto awayTeam = away.team_name();

    // TODO: These need to be filtered out, so they only contain games from division of the opponent (so home_games only contain games with homeTeam & a team from awayTeam's division, etc).
    auto home_games = home.get_games();
    auto away_games = away.get_games();

    std::vector<double> ppHome{}; std::vector<double> pkHome{};
    std::vector<double> ppTimesHome{}; std::vector<double> pkTimesHome{};
    std::vector<double> gfHome{}; std::vector<double> gaHome{}; std::vector<double> gtHome{};

    std::vector<double> ppAway{}; std::vector<double> pkAway{};
    std::vector<double> ppTimesAway{}; std::vector<double> pkTimesAway{};
    std::vector<double> gfAway{}; std::vector<double> gaAway{}; std::vector<double> gtAway{};

    auto team = homeTeam; // this variable is capture by reference by these lambdas. This is to make it possible to change it from homeTeam to awayTeam
    auto statsAccumulator = [&team](auto& acc, const auto& game) {
            auto [ppGoals, ppAtt] = game.power_play_stats(game.get_team_type(team));
            acc.powerPlayGoals += ppGoals;
            acc.powerPlayAttempts += ppAtt;
            auto [pkSuccess, pks] = game.penalty_kill_stats(game.get_team_type(team));
            acc.penaltyKillDefended += pkSuccess;
            acc.penaltyKillAttempts += pks;
            acc.goalsFor += game.goals_by(game.get_team_type(team));
            acc.goalsAgainst += game.goals_by(game.get_opponent_team_type(team));
            return acc;
    };
    // we only "look back" 5 games, and project the change from beginning of season, to now to the chart.
    // so the left-most item on the chart, will represent averages/stats that team had, 5 games ago
    // (those 5 games can be longer than 5 games ago, since we are _only_ looking at division stats, so
    // 5 games ago, in this context, reference to 5 games played against opponent division, ago)
    if(home_games.size() >= 5 && away_games.size() >= 5) {
        auto size = home.games_played();
        for(auto n = 1; n <= 5; ++n) {
            auto len = size - (5-n);
            auto [powerPlayGoals, powerPlayAttempts, penaltyKillDefended, penaltyKillAttempts, goalsFor, goalsAgainst] =
                    std::accumulate(home_games.cbegin(), home_games.cbegin()+len, IntermediateResult{0, 0, 0, 0, 0, 0}, statsAccumulator);

            ppHome.push_back(static_cast<double>(powerPlayGoals) / static_cast<double>(powerPlayAttempts));         // powerplay success ratio (not on a span average, but total average against division)
            pkHome.push_back(static_cast<double>(penaltyKillDefended) / static_cast<double>(penaltyKillAttempts));     // the same, but pks
            ppTimesHome.push_back(static_cast<double>(powerPlayAttempts) / static_cast<double>(len));        // the amount of powerplays/game average against division
            pkTimesHome.push_back(static_cast<double>(penaltyKillAttempts) / static_cast<double>(len));        // times in PK, on a per game average, against teams from division
            gfHome.push_back(static_cast<double>(goalsFor) / static_cast<double>(len));               // goals for, per game average
            gaHome.push_back(static_cast<double>(goalsAgainst) / static_cast<double>(len));           // goals against, per game average
            gtHome.push_back(static_cast<double>(goalsFor+goalsAgainst)/static_cast<double>(len));  // total goals, per game average
        }
        size = away.games_played();
        team = awayTeam;
        for(auto n = 1; n <= 5; ++n) {
            auto len = size - (5-n);
            auto [ppGoals, ppAttempts, pksDefended, pkAttempts, goalsFor, goalsAgainst] =
                    std::accumulate(away_games.cbegin(), away_games.cbegin()+len, IntermediateResult{0, 0, 0, 0, 0, 0}, statsAccumulator);
            ppAway.push_back(static_cast<double>(ppGoals) / static_cast<double>(ppAttempts));         // powerplay success ratio (not on a span average, but total average against division)
            pkAway.push_back(static_cast<double>(pksDefended) / static_cast<double>(pkAttempts));     // the same, but pks
            ppTimesAway.push_back(static_cast<double>(ppAttempts) / static_cast<double>(len));        // the amount of powerplays/game average against division
            pkTimesAway.push_back(static_cast<double>(pkAttempts) / static_cast<double>(len));        // times in PK, on a per game average, against teams from division
            gfAway.push_back(static_cast<double>(goalsFor) / static_cast<double>(len));               // goals for, per game average
            gaAway.push_back(static_cast<double>(goalsAgainst) / static_cast<double>(len));           // goals against, per game average
            gtAway.push_back(static_cast<double>(goalsFor+goalsAgainst)/static_cast<double>(len));  // total goals, per game average
        }
        auto format_from_percent = [&](auto& value) { return value*100.0; };
        std::transform(ppHome.begin(), ppHome.end(), ppHome.begin(), format_from_percent);
        std::transform(ppAway.begin(), ppAway.end(), ppAway.begin(), format_from_percent);
        std::transform(pkHome.begin(), pkHome.end(), pkHome.begin(), format_from_percent);
        std::transform(pkAway.begin(), pkAway.end(), pkAway.begin(), format_from_percent);
        auto homeDivision = teams_division_map()[homeTeam];
        auto awayDivision = teams_division_map()[awayTeam];
        QString div_vs_div = QString{"%1 vs %2"}.arg(homeDivision.c_str()).arg(awayDivision.c_str());

        pPowerPlay->set_title(QString("Power play - %1").arg(div_vs_div));
        pPenaltyKill->set_title(QString("Penalty Kill - %1").arg(div_vs_div));
        pTimesInPP->set_title(QString("Times in PP - %1").arg(div_vs_div));
        pTimesInPK->set_title(QString("Times in PK - %1").arg(div_vs_div));
        pGoalsFor->set_title(QString("Goals for - %1").arg(div_vs_div));
        pGoalsAgainst->set_title(QString("Goals Against - %1").arg(div_vs_div));
        pTotalGoals->set_title(QString("Total goals - %1").arg(div_vs_div));

        pPowerPlay->clear_and_update_new_data(homeTeam, awayTeam, ppHome, ppAway, LineChartRangeType::Percent);
        pPenaltyKill->clear_and_update_new_data(homeTeam, awayTeam, pkHome, pkAway, LineChartRangeType::Percent);
        pTimesInPP->clear_and_update_new_data(homeTeam, awayTeam, ppTimesHome, ppTimesAway);
        pTimesInPK->clear_and_update_new_data(homeTeam, awayTeam, pkTimesHome, pkTimesAway);
        pGoalsFor->clear_and_update_new_data(homeTeam, awayTeam, gfHome, gfAway);
        pGoalsAgainst->clear_and_update_new_data(homeTeam, awayTeam, gaHome, gaAway);
        pTotalGoals->clear_and_update_new_data(homeTeam, awayTeam, gtHome, gtAway);
    } else {
        // we need to make sure, that we only analyze the longest range that _both_ teams have played against each other's division
        // otherwise we will end up with a chart, where one team might have 5 data points, and the other only 3, for example.
        auto largest_possible_range_analysis = std::min(home_games.size(), away_games.size());
    }
}

void DivisionTab::set_chart_title_string_prefix(QString string)
{
    for(auto& chart : chartPointers)
        chart->set_title(QString("%1 %2").arg(chart->get_title()).arg(string));
}
