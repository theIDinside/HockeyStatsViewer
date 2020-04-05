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
    void update_chart_data(const TeamStats &home, const TeamStats &away) override;
    void register_connections(MainWindow& window);
private:
    QGridLayout m_layout;
    GameBoxesChart* lastFiveHome;
    GameBoxesChart* lastFiveAway;

    // Public TeamStatsTab interface
public:
    void set_chart_title_string_prefix(QString string) override;
    // TeamStatsTab interface
private:
    void hide_series_impl(SeriesType SType) override;
    void show_series_impl(SeriesType SType) override;
};
