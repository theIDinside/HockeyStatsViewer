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
    void set_chart_title_string_prefix(QString string) override;
private:
    QGridLayout m_layout;
    LineChart* pGoalsFor;
    LineChart* pGoalsAgainst;
    LineChart* pTotalGoals;
    QList<LineChart*> chartPointers;
    QTableView* mTable;

    // TeamStatsTab interface
public:

};
