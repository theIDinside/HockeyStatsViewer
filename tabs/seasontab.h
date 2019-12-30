#pragma once

#include "teamstatstab.h"

#include <QTableView>

class SeasonTab : public TeamStatsTab
{
    Q_OBJECT
public:
    explicit SeasonTab(QWidget* parent);
    ~SeasonTab() {}

    void update_chart_data(const TeamStats &home, const TeamStats &away) override;

private:
    QTableView* m_table;
};
