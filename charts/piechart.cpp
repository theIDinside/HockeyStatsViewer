#include "piechart.h"

PieChart::PieChart(QGraphicsItem* parent, Qt::WindowFlags wFlags) :
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    mCurrentSeries(nullptr)
{

}

PieChart::~PieChart()
{

}

void PieChart::changeSeries(QAbstractSeries *series)
{
    if(mCurrentSeries)
        removeSeries(mCurrentSeries);
    mCurrentSeries = series;
    addSeries(series);
    setTitle(series->name());
}

void PieChart::handleSliceClicked(QPieSlice *slice)
{

}
