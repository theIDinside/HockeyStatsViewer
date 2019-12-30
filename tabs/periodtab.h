#pragma once

#include "teamstatstab.h"

#include <QGridLayout>
#include <QPieSeries>

class PeriodTab : public TeamStatsTab
{
    Q_OBJECT
public:
    PeriodTab(QWidget* parent=nullptr);
    ~PeriodTab();
    void update_chart_data(const TeamStats &home, const TeamStats &away) override;

private:
    QGridLayout m_layout;
    LineChart* h_chart_gf;
    LineChart* h_chart_ga;
    LineChart* h_chart_sf;
    LineChart* h_chart_sa;

    LineChart* a_chart_gf;
    LineChart* a_chart_ga;
    LineChart* a_chart_sf;
    LineChart* a_chart_sa;
    QList<LineChart*> chartPtrs;

};
