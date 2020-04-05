#include "areachart.h"
#include <QDebug>
#include <cassert>
AreaChart::AreaChart(const std::string& title, QWidget* parent) : QGraphicsView(new QGraphicsScene, parent), title{title.c_str()}, m_tooltip(nullptr), m_saved_tooltips{}
{
    setDragMode(QGraphicsView::NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    chart = new QChart;
    chart->setTitle(title.c_str());
    chart->createDefaultAxes();
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->setAnimationDuration(350);
    scene()->addItem(chart);
    this->setMouseTracking(true);
    this->setMinimumHeight(275);

    bottomBoundary = new QLineSeries;
    topBoundary = new QLineSeries;
    areaSeries = new QAreaSeries;
}

void AreaChart::update_chart(const std::string& title, std::vector<double> upper_bound, std::vector<double> bottom_bound)
{
    chart->removeAllSeries();

    bottomBoundary = new QLineSeries;
    topBoundary = new QLineSeries;
    topTrackingBoundary = new QLineSeries;
    topBoundary->setName("UPPER");
    topTrackingBoundary->setName("UPPER");
    chart->setTitle(title.c_str());
    if(bottom_bound.size() == 0) {
        std::vector<double> BottomBoundary{};
        auto x_step = 0;
        for(const auto& value : upper_bound) {
            bottomBoundary->append(x_step, 0);
            topBoundary->append(x_step, value);
            topTrackingBoundary->append(x_step, value);
            x_step++;
        }
    } else {
        assert(bottom_bound.size() == upper_bound.size());
    }
    areaSeries = new QAreaSeries(topBoundary, bottomBoundary);
    areaSeries->setName(title.c_str());
    QPen pen(0x059605);
    pen.setWidth(3);
    areaSeries->setPen(pen);
    QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));
    gradient.setColorAt(0.0, 0x3cc63c);
    gradient.setColorAt(1.0, 0x26f626);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    areaSeries->setBrush(gradient);
    chart->addSeries(areaSeries);
    chart->addSeries(topTrackingBoundary);
    connect(topTrackingBoundary, &QLineSeries::hovered, this, &AreaChart::ac_tooltip);
    connect(areaSeries, &QAreaSeries::hovered, this, &AreaChart::ac_tooltip);
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, 60.0);
    chart->axes(Qt::Vertical).first()->setRange(0, 100.0);
    chart->setAcceptHoverEvents(true);
    setRenderHint(QPainter::Antialiasing);
}

void AreaChart::ac_tooltip(QPointF point, bool state)
{
    qDebug() << "Hovering over top boundary";
    if (!m_tooltip)
        m_tooltip = new LineChartDataToolTip(chart);
    if (state) {
        auto sen = sender();
        QLineSeries* series = qobject_cast<QLineSeries*>(sen);
        if(series != nullptr) {
            auto name = series->name();
            if(name == "UPPER") {
                qDebug() << "Are we in area series or line series?";
                auto alreadyShown = false;
                if(!alreadyShown) {
                    m_tooltip->setText(QString("%1, %2").arg(point.x()).arg(point.y()));
                    m_tooltip->setAnchor(point);
                    m_tooltip->setZValue(11);
                    m_tooltip->updateGeometry();
                    m_tooltip->show();
                }
            }
        }
    } else {
        m_tooltip->hide();
    }
}

void AreaChart::resizeEvent(QResizeEvent *event)
{
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
         chart->resize(event->size());
         const auto tooltips = m_saved_tooltips;
         for(auto& ptr : tooltips) {
             ptr->updateGeometry();
         }
    }
    QGraphicsView::resizeEvent(event);
}

void AreaChart::mouseMoveEvent(QMouseEvent *event)
{
    // Do stuff when mouse is moved...
    QGraphicsView::mouseMoveEvent(event);
}
