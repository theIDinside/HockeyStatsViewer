#include "goalstab.h"

#include <data/stattable.h>
#include <utils.h>
#include <QHeaderView>

GoalsTab::GoalsTab(QWidget* parent) : TeamStatsTab(parent), m_layout{this}, pGoalsFor{nullptr}, pGoalsAgainst{nullptr}
{
    LineChart* gf = new LineChart("Goals for");
    LineChart* ga = new LineChart("Goals against");
    LineChart* gt = new LineChart("Total goals");

    pGoalsFor = gf;
    pGoalsAgainst = ga;
    pTotalGoals = gt;
    chartPointers << pGoalsFor << pGoalsAgainst << pTotalGoals;
    mTable = new QTableView(this);
    mTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_layout.addWidget(gf,0,0);
    m_layout.addWidget(ga, 0, 1);
    m_layout.addWidget(gt, 1, 0);
    auto l = new QHBoxLayout;
    l->addWidget(mTable);
    m_layout.addLayout(l, 1,1, Qt::Alignment(Qt::AlignmentFlag::AlignTop));
    // m_layout.addWidget(mTable, 1,1);
    this->setLayout(&m_layout);
}

void GoalsTab::update_chart_data(const TeamStats &home, const TeamStats &away)
{
    auto homeGF = home.goals_for_avg(Span::Five);
    auto awayGF = away.goals_for_avg(Span::Five);
    auto homeGA = home.goals_against_avg(Span::Five);
    auto awayGA = away.goals_against_avg(Span::Five);

    auto homeTeam = home.team_name();
    auto awayTeam = away.team_name();

    auto HSGF = home.goals_for_avg(Span::Season);
    auto homeSeasonGF = home.season_avg_last_x_games(5, [&](auto teamName, auto begin, auto end, auto divisor) {
        double goals_made = std::accumulate(begin, end, 0.0, [&](auto& acc, const GameModel& game) {
            return acc + (double)game.goals_by(game.get_team_type(teamName), GoalType::RegularGame);
        });
        return goals_made / (double) divisor;
    });
    auto awaySeasonGF = away.season_avg_last_x_games(5, [&](auto teamName, auto begin, auto end, auto divisor) {
        double goals_made = std::accumulate(begin, end, 0.0, [&](auto& acc, const GameModel& game) {
            return acc + (double)game.goals_by(game.get_team_type(teamName), GoalType::RegularGame);
        });
        return goals_made / (double) divisor;
    });

    auto homeSeasonGA = home.season_avg_last_x_games(5, [&](auto teamName, auto begin, auto end, auto divisor) {
        double goals_against = std::accumulate(begin, end, 0.0, [&](auto& acc, const GameModel& game) {
           return acc + (double)game.goals_by(game.get_opponent_team_type(teamName), GoalType::RegularGame);
        });
        return goals_against / (double)divisor;
    });
    auto awaySeasonGA = away.season_avg_last_x_games(5, [&](auto teamName, auto begin, auto end, auto divisor) {
        double goals_against = std::accumulate(begin, end, 0.0, [&](auto& acc, const GameModel& game) {
           return acc + (double)game.goals_by(game.get_opponent_team_type(teamName), GoalType::RegularGame);
        });
        return goals_against / (double)divisor;
    });

    std::vector<std::vector<double>> GFData{homeGF, homeSeasonGF, awayGF, awaySeasonGF};
    std::vector<std::vector<double>> GAData{homeGA, homeSeasonGA, awayGA, awaySeasonGA};

    QList<QString> tableRowHeaders;
    tableRowHeaders << "Empty net goals" << "Empty net let ups" << "Games with PP goals" << "Games with PK Letups";
    mTable->setFixedHeight(mTable->verticalHeader()->length()+mTable->horizontalHeader()->height()+2);

    StatTable* model = new StatTable    (tableRowHeaders, mTable);
    auto [hENGames, hWon] = home.empty_net_scoring();
    auto [aENGames, aWon] = away.empty_net_scoring();
    auto [hENLGames, hLost] = home.empty_net_letups();
    auto [aENLGames, aLost] = away.empty_net_letups();
    auto [hGamesWPPGoals, hTotalGames] = home.games_with_pp_goals();
    auto [aGamesWPPGoals, aTotalGames] = away.games_with_pp_goals();

    auto [hGamesWPKLetUps, hTotalGames1] = home.games_with_pk_letups();
    auto [aGamesWPKLetUps, aTotalGames1] = away.games_with_pk_letups();


    std::vector<double> hTableData{};
    std::vector<double> aTableData{};
    hTableData.push_back(divide_integers(hENGames, hWon));
    hTableData.push_back(divide_integers(hENLGames, hLost));
    hTableData.push_back(divide_integers(hGamesWPPGoals, hTotalGames));
    hTableData.push_back(divide_integers(hGamesWPKLetUps, hTotalGames));


    aTableData.push_back(divide_integers(aENGames, aWon));
    aTableData.push_back(divide_integers(aENLGames, aLost));
    aTableData.push_back(divide_integers(aGamesWPPGoals, aTotalGames1));
    aTableData.push_back(divide_integers(aGamesWPKLetUps, aTotalGames1));

    model->populate_data(home.team_name().c_str(), away.team_name().c_str(), hTableData, aTableData);
    mTable->setModel(model);
    mTable->horizontalHeader()->setVisible(true);
    mTable->verticalHeader()->setVisible(true);
    std::vector<double> totalHome{};
    std::vector<double> totalAway{};
    auto size = homeGF.size();
    for(auto i = size_t{0}; i < size; ++i) {
        totalHome.push_back(homeGF[i] + homeGA[i]);
        totalAway.push_back(awayGF[i] + awayGA[i]);
    }
    std::vector<std::string> seriesNames{home.team_name(), home.team_name() + " Season", away.team_name(), away.team_name() + " Season"};
    // pGoalsFor->clear_and_update_new_data(home.team_name(), away.team_name(), homeGF, awayGF);
    pGoalsFor->clear_and_update_new_multi_series_data(seriesNames, GFData);
    //pGoalsAgainst->clear_and_update_new_data(home.team_name(), away.team_name(), homeGA, awayGA);
    pGoalsAgainst->clear_and_update_new_multi_series_data(seriesNames, GAData);
    pTotalGoals->clear_and_update_new_data(home.team_name(), away.team_name(), totalHome, totalAway);
}

void GoalsTab::set_chart_title_string_prefix(QString string)
{
    for(auto& chart : chartPointers)
        chart->set_title(QString("%1 %2").arg(chart->get_title()).arg(string));
}

void GoalsTab::hide_series_impl(SeriesType SType)
{
    if(SType == SeriesType::SeasonSeries) {
        for(auto p : chartPointers) {
            p->hide_season_series();
        }
    }
}

void GoalsTab::show_series_impl(SeriesType SType)
{
    if(SType == SeriesType::SeasonSeries) {
        for(auto p : chartPointers) {
            p->show_season_series();
        }
    }
}
