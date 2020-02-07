#pragma once

#include <charts/linechart.h>
#include "teamstats.h"

#include <QObject>
#include <QWidget>
#include <QDebug>

using namespace QtCharts;

enum SeriesType {
    SeasonSeries,
    Span,
    Period
};

class TeamStatsTab : public QWidget
{
    Q_OBJECT
public:
    explicit TeamStatsTab(QWidget* parent=nullptr);
    virtual ~TeamStatsTab() {
        qDebug() << "Virtual destructor for team statistics tab";
    }

    virtual void set_chart_title_string_prefix(QString string) = 0;
    void hide_series(SeriesType type);
    void show_series(SeriesType type);
private:
    virtual void hide_series_impl(SeriesType SType) = 0;
    virtual void show_series_impl(SeriesType SType) = 0;
public slots:
    virtual void update_chart_data(const TeamStats& home, const TeamStats& away) = 0;
public:
    using ST = GameModel::SpecialTeamType;
    using RType = LineChartRangeType;
};
