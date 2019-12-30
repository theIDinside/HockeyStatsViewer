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

    pGoalsFor->clear_and_update_new_data(home.team_name(), away.team_name(), homeGF, awayGF);
    pGoalsAgainst->clear_and_update_new_data(home.team_name(), away.team_name(), homeGA, awayGA);
    pTotalGoals->clear_and_update_new_data(home.team_name(), away.team_name(), totalHome, totalAway);
}
