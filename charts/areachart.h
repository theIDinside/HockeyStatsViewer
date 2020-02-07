#pragma once
#include "linechartdatatooltip.h"

#include <QGraphicsView>
#include <QObject>
#include <QWidget>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QAreaSeries>
#include <QString>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QMouseEvent;
class QResizeEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE
class AreaChart : public QGraphicsView
{
    Q_OBJECT
public:
    AreaChart(const std::string& title, QWidget* parent=nullptr);
    void update_chart(const std::string& title, std::vector<double> upper_bound, std::vector<double> bottom_bound={});
private:
    QChart* chart;
    QString title;

    QLineSeries* bottomBoundary;
    QLineSeries* topBoundary;
    QLineSeries* topTrackingBoundary;
    QAreaSeries* areaSeries;
    LineChartDataToolTip* m_tooltip;
    QList<LineChartDataToolTip*> m_saved_tooltips;

public slots:
    void ac_tooltip(QPointF point, bool state); // called when we hover over a series/line in the chart

protected:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

};

AreaChart* GameDistribution(const std::string& title);
