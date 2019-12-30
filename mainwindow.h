#pragma once

#include "gamelistitemmodel.h"
#include "mdbconnection.h"
#include "teamstats.h"
#include "tabs/teamstatstab.h"
#include <QActionGroup>
#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QStringListModel>
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool hook_db_connection(std::unique_ptr<MDbConnection> conn);
    void show_popup();

public slots:
    void on_mLoadGamesToday_released();
    void on_mGamesTodayListView_clicked(const QModelIndex &index);
    void on_mDatePicker_clicked(const QDate &date);
    void on_mUpdateCharts_clicked();

    void game_data_popup(int gameID);


signals:
    void gameSelectionChanged(const TeamStats& home, const TeamStats& away);

private:

    void createMenu();

    Ui::MainWindow *ui;
    std::unique_ptr<MDbConnection> m_connection;
    QComboBox* m_gamesTodayList;
    QLabel* m_gamesTodayLabel;
    std::vector<GameInfoModel> m_gamesToday;
    GameListItemModel* m_gamesTodayModel;
    QTabWidget* m_tabContainer;
    QList<TeamStatsTab*> m_statsTabs;
    bool popup_shown;
    QActionGroup* m_spanSettingGroup;
    QAction* m_actionSpanSet5;
    QAction* m_actionSpanSet10;
    QAction* m_actionSpanSetSeason;


    void connectTabSignals();
};
