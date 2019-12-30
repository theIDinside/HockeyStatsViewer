#include "tabs/goalstab.h"
#include "mainwindow.h"
#include "teamstats.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include "gamelistitemmodel.h"
#include "tabs/specialteamtab.h"
#include <tabs/divisiontab.h>
#include <tabs/lastfivegamestab.h>
#include <tabs/periodtab.h>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_gamesToday{}
{
    popup_shown = false;
    ui->setupUi(this);
    m_tabContainer = new QTabWidget;

    QScrollArea* specialTeamsScrollArea = new QScrollArea;
    specialTeamsScrollArea->setLayout(new QVBoxLayout);
    auto specialTeams = new SpecialTeamTab(m_tabContainer);
    specialTeamsScrollArea->setWidget(specialTeams);
    specialTeamsScrollArea->setWidgetResizable(true);
    m_tabContainer->addTab(specialTeamsScrollArea, "Special Teams");


    QScrollArea* goalsTabScrollArea = new QScrollArea;
    goalsTabScrollArea->setLayout(new QVBoxLayout);
    auto goalsTab = new GoalsTab(m_tabContainer);
    goalsTabScrollArea->setWidget(goalsTab);
    goalsTabScrollArea->setWidgetResizable(true);
    m_tabContainer->addTab(goalsTabScrollArea, "Goals");

//    m_tabContainer->addTab(scrollArea, "Scroll area");

    QScrollArea* divisionScrollArea = new QScrollArea;
    divisionScrollArea->setLayout(new QVBoxLayout);
    auto divisionTab = new DivisionTab(m_tabContainer);
    divisionScrollArea->setWidget(divisionTab);
    divisionScrollArea->setWidgetResizable(true);
    m_tabContainer->addTab(divisionScrollArea, "Division stats");

    QScrollArea* periodTabScrollArea = new QScrollArea;
    periodTabScrollArea->setLayout(new QVBoxLayout);
    auto periodTab = new PeriodTab(m_tabContainer);
    periodTabScrollArea->setWidget(periodTab);
    periodTabScrollArea->setWidgetResizable(true);
    m_tabContainer->addTab(periodTabScrollArea, "Period stats");

    QScrollArea* lastFiveScrollArea = new QScrollArea;
    lastFiveScrollArea->setLayout(new QVBoxLayout);
    auto lastFiveTab = new LastFiveGamesTab(m_tabContainer);
    lastFiveTab->register_connections(*this);
    lastFiveScrollArea->setWidget(lastFiveTab);
    lastFiveScrollArea->setWidgetResizable(true);
    m_tabContainer->addTab(lastFiveScrollArea, "Last 5 games");

    auto frameLayout = new QVBoxLayout();
    frameLayout->setSpacing(0);
    frameLayout->setContentsMargins(0,0,0,0);
    frameLayout->addWidget(m_tabContainer);
    ui->mContentFrame->setLayout(frameLayout);

    // Resource management, used when destructing.
    m_statsTabs << specialTeams << goalsTab << divisionTab << periodTab << lastFiveTab;

    connectTabSignals();
    createMenu();
}

MainWindow::~MainWindow()
{
    for(auto pointer : m_statsTabs) delete pointer;
    delete ui;
}

bool MainWindow::hook_db_connection(std::unique_ptr<MDbConnection> conn)
{
    if(!m_connection) {
        m_connection = std::move(conn);
        return true;
    }
    return true;
}

void MainWindow::on_mLoadGamesToday_released()
{
    auto games = m_connection->getTodaysGames();
    std::copy(games.begin(), games.end(), std::back_inserter(m_gamesToday));
    m_gamesTodayModel = new GameListItemModel(std::move(games), this);
    ui->mGamesTodayListView->setModel(m_gamesTodayModel);
}

void MainWindow::on_mGamesTodayListView_clicked(const QModelIndex &index)
{
    auto retrievedData = ui->mGamesTodayListView->model()->data(index, GameListItemModel::Role::GetGameID);
    if(!retrievedData.isNull()) {
        auto it = std::find_if(m_gamesToday.begin(), m_gamesToday.end(), [&](const auto& gameInfo) {
            auto gameId = retrievedData.toInt();
            return gameInfo.m_game_id == gameId;
        });
        if(it != m_gamesToday.end()) {
            auto away_games = m_connection->getGames(it->away_team());
            auto home_games = m_connection->getGames(it->home_team());
            TeamStats ts_away{it->away_team(), std::move(away_games)};
            TeamStats ts_home{it->home_team(), std::move(home_games)};
            emit gameSelectionChanged(ts_home, ts_away); // sending off data to all tabs, so they can update with newly selected team stats
        } else {
            std::cout << "Found no games!! " << retrievedData.toInt() << "."  << std::endl;
        }
    }


    // TODO: Emit the gameID for the game clicked, to the appropriate signal
    // TODO: Then request data from backend
    // TODO: init charts, set various charts data from TeamStats object
}

void MainWindow::createMenu()
{
    m_spanSettingGroup = new QActionGroup(this);
    auto spanSettingMenu = menuBar()->addMenu("Span analyzed");
    m_actionSpanSet5        = new QAction("Set to 5", this);
    m_actionSpanSet10       = new QAction("Set to 10", this);
    m_actionSpanSetSeason   = new QAction("Set to season", this);

    m_actionSpanSet5->setCheckable(true);
    m_actionSpanSet10->setCheckable(true);
    m_actionSpanSetSeason->setCheckable(true);

    m_spanSettingGroup->addAction(m_actionSpanSet5);
    m_spanSettingGroup->addAction(m_actionSpanSet10);
    m_spanSettingGroup->addAction(m_actionSpanSetSeason);
    m_actionSpanSet5->setChecked(true);
    m_spanSettingGroup->setExclusive(true);

    spanSettingMenu->addAction(m_actionSpanSet5);
    spanSettingMenu->addAction(m_actionSpanSet10);
    spanSettingMenu->addAction(m_actionSpanSetSeason);
    // TODO: connect the actions their respective signals/slots

}

void MainWindow::connectTabSignals()
{
    // connect(this, &MainWindow::gameSelectionChanged, specialTeams, &TeamStatsTab::update_chart_data);
    // connect(this, &MainWindow::gameSelectionChanged, goalsTab, &TeamStatsTab::update_chart_data);
    for(auto tabPtr : m_statsTabs) {
        connect(this, &MainWindow::gameSelectionChanged, tabPtr, &TeamStatsTab::update_chart_data);
    }
}

std::pair<std::chrono::system_clock::time_point,std::chrono::system_clock::time_point>  day_range_from_QDate(const QDate& date) {
    auto dt_begin = QDateTime{date};
    std::cout << "DT begin: " << dt_begin.toString().toStdString() << std ::endl;
    auto d = date.addDays(1);
    auto dt_end = QDateTime{d};
    std::cout << "DT End:" << dt_end.toString().toStdString() << std::endl;
    auto t = dt_begin.toTime_t();
    auto t2 = dt_end.toTime_t();

    auto tp1 = std::chrono::system_clock::from_time_t(t);
    auto tp2 = std::chrono::system_clock::from_time_t(t2);
    return std::make_pair(tp1, tp2);
}

void MainWindow::on_mDatePicker_clicked(const QDate &date)
{
    // Get games for date
    qDebug() << date;
    m_gamesToday.clear();
    auto games = m_connection->getGamesAtDate(day_range_from_QDate(date));
    std::copy(games.begin(), games.end(), std::back_inserter(m_gamesToday));
    m_gamesTodayModel = new GameListItemModel(std::move(games), this);
    ui->mGamesTodayListView->setModel(m_gamesTodayModel); // widget takes ownership over m_gamesTodayModel pointer... do not delete it manually.
}

void MainWindow::on_mUpdateCharts_clicked()
{
    std::cout << "Button clicked" << std::endl;
    popup_shown = !popup_shown;
    show_popup();
}

void MainWindow::game_data_popup(int gameID)
{
    std::cout << "Do: Pop up window for game data for game with id: " << gameID << std::endl;
}

void MainWindow::show_popup()
{
    if(popup_shown) {
        // show pop up
        std::cout << "Show popup window!" << std::endl;
    } else {
        // hide popup
        std::cout << "Hide popup window!" << std::endl;
    }
}
