#include "teamstatstab.h"
#include "teamstats.h"

#include <QGridLayout>
#include <gameboxeschart.h>

#pragma once

class LastFiveGamesTab : public TeamStatsTab
{
    Q_OBJECT
public:
    LastFiveGamesTab(QWidget* parent=nullptr);

    void hook_db(std::shared_ptr<MDbConnection> conn);
    std::shared_ptr<MDbConnection> mptr_db;
    void update_chart_data(const TeamStats &home, const TeamStats &away) override;
    void register_connections(MainWindow& window);
private:
    QGridLayout m_layout;
    GameBoxesChart* lastFiveHome;
    GameBoxesChart* lastFiveAway;
};
