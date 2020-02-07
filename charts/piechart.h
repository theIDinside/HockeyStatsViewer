#pragma once

#include <QGraphicsView>
#include <QObject>
#include <QtCharts/QChart>


QT_CHARTS_BEGIN_NAMESPACE
class QAbstractSeries;
class QPieSlice;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class PieChart : public QChart
{
    Q_OBJECT
public:
    explicit PieChart(QGraphicsItem* parent=nullptr, Qt::WindowFlags wFlags = 0);
    ~PieChart();
    void changeSeries(QAbstractSeries* series);

public slots:
    void handleSliceClicked(QPieSlice* slice);

private:
    QAbstractSeries* mCurrentSeries;

};
