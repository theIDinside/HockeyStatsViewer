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
private:
    QGridLayout m_layout;
    QList<LineChart*> chartPointers;
    LineChart* pPowerPlay;
    LineChart* pPenaltyKill;
    LineChart* pPowerPlayPerGame;
    LineChart* pPenaltyKillPerGame;

    // TeamStatsTab interface
public:
    void update_chart_data(const TeamStats& home, const TeamStats& away) override;
    void set_chart_title_string_prefix(QString string) override;
};
