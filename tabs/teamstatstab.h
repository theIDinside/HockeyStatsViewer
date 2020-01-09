#pragma once

#include <charts/linechart.h>
#include "teamstats.h"

#include <QObject>
#include <QWidget>
#include <QDebug>

using namespace QtCharts;


class TeamStatsTab : public QWidget
{
    Q_OBJECT
public:
    explicit TeamStatsTab(QWidget* parent=nullptr);
    virtual ~TeamStatsTab() {
        qDebug() << "Virtual destructor for team statistics tab";
    }

    virtual void set_chart_title_string_prefix(QString string) = 0;
public slots:
    virtual void update_chart_data(const TeamStats& home, const TeamStats& away) = 0;
public:
    using Span = TeamStats::Span;
    using ST = GameModel::SpecialTeamType;
    using RType = LineChartRangeType;
};
