#pragma once

#include "teamstatstab.h"
#include <QGridLayout>
#include <QTableView>

class GoalsTab : public TeamStatsTab
{
    Q_OBJECT
public:
    GoalsTab(QWidget* parent=nullptr);
    ~GoalsTab() {
        delete pGoalsFor;
        delete pGoalsAgainst;
        delete pTotalGoals;
    }
    void update_chart_data(const TeamStats& home, const TeamStats& away) override;

private:
    QGridLayout m_layout;
    LineChart* pGoalsFor;
    LineChart* pGoalsAgainst;
    LineChart* pTotalGoals;
    QTableView* mTable;
};
