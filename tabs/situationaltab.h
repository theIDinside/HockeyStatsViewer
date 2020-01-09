#pragma once
#include <QObject>
#include <tabs/teamstatstab.h>
/**
 * Get following statistics for home team:
 *  Amount of PP goals for team, when they play at home
 *  Amount of PK goals team lets up, when they play at home
 *  SF when team plays at home
 *  SA when team plays at home
 *  PP times when at home
 *  PK times when at home
 *  Total wins at home
 *  How many games have been won at home, against opponents with equal or better away statistics than opponent
 *
 * @brief The SituationalTab class
 */

class SituationalTab : public TeamStatsTab
{
    Q_OBJECT
public:
    SituationalTab(QWidget* parent=nullptr);

    // TeamStatsTab interface
public slots:
    void update_chart_data(const TeamStats &home, const TeamStats &away) override;
};
