#include "lastfivegamestab.h"

LastFiveGamesTab::LastFiveGamesTab(QWidget* parent) : TeamStatsTab(parent)
{
    lastFiveHome = new GameBoxesChart(this);
    lastFiveAway = new GameBoxesChart(this);
    lastFiveAway->setFixedHeight(300);
    lastFiveHome->setFixedHeight(300);
    m_layout.setAlignment(Qt::AlignTop | Qt::AlignCenter);
    m_layout.addWidget(lastFiveHome, 0, 0);
    m_layout.addWidget(lastFiveAway, 0, 1);
    setLayout(&m_layout);
}

void LastFiveGamesTab::hook_db(std::shared_ptr<MDbConnection> conn)
{
    mptr_db = conn;
}

void LastFiveGamesTab::update_chart_data(const TeamStats &home, const TeamStats &away)
{
    using TC = TrendComparison;
    using std::pair;

    lastFiveAway->update_data(away);
    lastFiveHome->update_data(home);
/*
    auto HomeTeam = home.team_name();
    auto AwayTeam = away.team_name();

    auto last5GamesHomeTeam = home.games_range_from_back(5);
    auto last5GamesAwayTeam = away.games_range_from_back(5);

    std::vector<pair<TC, TC>> htComparisons{};
    std::vector<pair<TC, TC>> atComparisons{};
    auto [home_it, end] = last5GamesHomeTeam;
    for(; home_it != end; home_it++) {
        auto tc = home.compare_game_to_trend_stats(*home_it);
        auto opponent = home_it->get_opponent_team_name(home.team_name());
        auto gamesOfOpp = mptr_db->getGames(opponent);
        TeamStats oppTS{opponent, std::move(gamesOfOpp)};
        auto oppTC = oppTS.compare_game_to_trend_stats(*home_it);

        if(home_it->winning_team() == HomeTeam) {
            // find out if:
            // A. HomeTeam did better than it's trend stats (tc)
            // B. Opponent did better/worse than it's trend stats (oppTC)
            // C. Analyze Opponents other losses and compare those GameStatistics to this game's GameStatistics (use GameStatistics::from(*home_it), to construct GS object)
        }

        htComparisons.emplace_back(tc, oppTC);
    }
    auto[it, e] = last5GamesAwayTeam;
    for(; it != e; it++) {
        auto tc = away.compare_game_to_trend_stats(*it);
        auto opponent = it->get_opponent_team_name(away.team_name());
        auto gamesOfOpp = mptr_db->getGames(opponent);
        TeamStats oppTS{opponent, std::move(gamesOfOpp)};
        auto oppTC = oppTS.compare_game_to_trend_stats(*it);
        atComparisons.emplace_back(tc, oppTC);
    }
    */
}

void LastFiveGamesTab::register_connections(MainWindow &window)
{
    lastFiveAway->register_connections(window);
    lastFiveHome->register_connections(window);
}
