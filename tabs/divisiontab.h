#pragma once
#include "teamstatstab.h"
#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QGridLayout>

class DivisionTab : public TeamStatsTab
{
    Q_OBJECT
public:
    DivisionTab(QWidget* parent = nullptr);
    ~DivisionTab() {

    }
    void update_chart_data(const TeamStats &home, const TeamStats &away) override;
    void set_chart_title_string_prefix(QString string) override;
private:
    QGridLayout m_layout;
    LineChart* pPowerPlay;
    LineChart* pPenaltyKill;
    LineChart* pTimesInPP;
    LineChart* pTimesInPK;
    LineChart* pGoalsFor;
    LineChart* pGoalsAgainst;
    LineChart* pTotalGoals;
    LineChart* pShotEfficiency;
    QList<LineChart*> chartPointers;

    // TeamStatsTab interface
public:

};
