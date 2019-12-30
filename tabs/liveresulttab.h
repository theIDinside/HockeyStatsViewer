#pragma once
#include "teamstatstab.h"

#include <QObject>

/* Tab that takes input of current live standing in a game and returns some analytic data from previous games with similar-ish results */
class LiveResultTab : public TeamStatsTab
{
    Q_OBJECT
public:
    LiveResultTab(QWidget* parent=nullptr);
};
