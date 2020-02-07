#include "liveresulttab.h"

LiveResultTab::LiveResultTab(QWidget* parent) : TeamStatsTab(parent), mRightPaneLayout{}, mAnswers{}
{
    mMainLayout         = new QHBoxLayout(this);
    mLeftPaneLayout     = new QVBoxLayout();
    mRightPaneLayout    = new QGridLayout();
    // goalDist = new AreaChart("Goal Distribution");
    mInputWidget        = new RequestInputWidget(this);
    mLeftPaneLayout->addWidget(mInputWidget, 0, Qt::AlignLeft | Qt::AlignTop);
    mMainLayout->addLayout(mLeftPaneLayout);
 //   mRightPaneLayout->addWidget(goalDist);
    // mRightPaneLayout->addWidget(/* SOME WIDGET */, 0, 1);
    // mRightPaneLayout->addWidget(/* SOME WIDGET */, 1, 1);
    mMainLayout->addLayout(mRightPaneLayout, 1);
    setLayout(mMainLayout);
}

void LiveResultTab::register_inputs_with_main(MainWindow &window)
{
    mInputWidget->register_mainwindow_connection(window);
}

void LiveResultTab::add_answer(const std::string &text)
{
    auto row = mAnswers.size() / 2;
    auto col = mAnswers.size() % 2;
    QLabel* l = new QLabel(text.c_str());
    l->setMinimumSize(100, 25);
    mAnswers << l;
    mRightPaneLayout->addWidget(l, row, col);
}

void LiveResultTab::set_chart_title_string_prefix(QString string)
{
    qDebug() << "Live Result Tab: LiveResultTab::set_chart_title_string_prefix(QString string) not yet implemented";
}

void LiveResultTab::update_chart_data(const TeamStats &home, const TeamStats &away)
{
    GameTime g{15, 1, 1};
    Standing s{home.team_name(), away.team_name(), 1, 0, g};
    analyze_with_standing(home, away, s);
}

void LiveResultTab::analyze_with_standing(const TeamStats &home, const TeamStats &away, Standing standing)
{
    auto homeTeam = home.team_name();
    auto awayTeam = away.team_name();

    auto homeTeamResults = home.get_games_which_had_score(standing.mHome, standing.mAway);
    auto awayTeamResults = away.get_games_which_had_score(standing.mAway, standing.mHome);

    std::pair<int, int> WinLossRatio{0, 0};
    std::pair<int, int> WinLossWTime{0, 0};
    std::for_each(homeTeamResults.cbegin(), homeTeamResults.cend(), [&](const GameModel& game) {
        auto&[win, loss] = WinLossRatio; auto&[timeWin, timeLoss] = WinLossWTime;
        if(game.winning_team() == homeTeam) win++;
        else loss++;
        if(auto timeOfPeriod = game.had_standing(homeTeam, standing.mHome, standing.mAway); timeOfPeriod) {
            if(timeOfPeriod <= standing.mGameTime) {
                if(game.winning_team() == homeTeam) timeWin++; // game had same score, at same time/before time in current game, and won...
                else timeLoss++;    // ... as above, but lost
            }
        }
    });

    std::cout << "Games with standing " << standing << " where " << homeTeam << " won: " << WinLossWTime.first << ", lost: " << WinLossWTime.second << std::endl;

    auto ratioWinLossH = std::accumulate(homeTeamResults.cbegin(), homeTeamResults.cend(), std::pair<int, int>{0, 0}, [&homeTeam](auto& acc, const GameModel& game) {
        auto& [win, loss] = acc;
        if(game.winning_team() == homeTeam) win++;
        else loss++;
        return acc;
    });
}

void LiveResultTab::hide_series_impl(SeriesType SType)
{
    qDebug() << "Virtual member function: void LiveResultTab::hide_series_impl(SeriesType SType) not implemented";
}

void LiveResultTab::show_series_impl(SeriesType SType)
{
    qDebug() << "void LiveResultTab::show_series_impl(SeriesType SType) not yet implemented";
}