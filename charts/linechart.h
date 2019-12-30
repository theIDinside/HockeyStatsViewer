#pragma once

#include "linechartdatatooltip.h"

#include <QtWidgets/QGraphicsView>
#include <QtCharts/QChartGlobal>

#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QObject>
#include <QWidget>
#include <memory>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QMouseEvent;
class QResizeEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

enum LineChartRangeType {
    Percent,
    Value
};

using namespace QtCharts;

class LineChart : public QGraphicsView
{
    Q_OBJECT
public:
    static constexpr auto HOMEID = 100;
    static constexpr auto AWAYID = 200;
    LineChart(std::string title, QWidget* parent = nullptr);
    void clear_and_update_new_data(const std::string& home, const std::string& away,  std::vector<double> home_chart_data, std::vector<double> away_chart_data, LineChartRangeType type=LineChartRangeType::Value);
    void clear_and_update_new_multi_series_data(const std::vector<std::string>& series_names, std::vector<std::vector<double>> series_vector, LineChartRangeType type=LineChartRangeType::Value);
    // TODO:
    friend QPointF translate_to_actual_data(LineChart& lineChart, QPointF chart_point, std::size_t series_index);
    void set_title(const std::string& title);
    void set_title(const QString& title);
protected:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

public slots:
    void keepTooltip();                      // called when we click on a series/line in the chart
    void tooltip(QPointF point, bool state); // called when we hover over a series/line in the chart

private:
    std::string m_title;
    QChart* m_chart;
    QString m_home_team;
    QString m_away_team;
    LineChartDataToolTip* m_tooltip;
    QList<LineChartDataToolTip*> m_saved_tooltips;
    QList<QLineSeries*> m_series;
    std::vector<std::vector<double>> multi_series;

    std::vector<double> m_series_data_home; // we fetch the data from here, when we hover over the chart, otherwise we will get noise values between integer steps of the x-axis.
    std::vector<double> m_series_data_away;
    LineChartRangeType valueType;
};
