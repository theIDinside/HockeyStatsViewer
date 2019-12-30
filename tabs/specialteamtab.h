#pragma once

#include "teamstatstab.h"
#include <QGridLayout>

using namespace QtCharts;

class SpecialTeamTab : public TeamStatsTab
{
    Q_OBJECT
public:
    explicit SpecialTeamTab(QWidget* parent=nullptr);
    ~SpecialTeamTab() {
        qDebug() << "Destroying special teams tab";
        delete pPowerPlay;
        delete pPenaltyKill;
        delete pPowerPlayPerGame;
        delete pPenaltyKillPerGame;
    }
    void update_chart_data(const TeamStats& home, const TeamStats& away) override;
private:
    QGridLayout m_layout;
    LineChart* pPowerPlay;
    LineChart* pPenaltyKill;
    LineChart* pPowerPlayPerGame;
    LineChart* pPenaltyKillPerGame;
};
