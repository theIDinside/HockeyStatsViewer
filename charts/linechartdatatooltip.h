#pragma once

#include <QtCharts/QChartGlobal>
#include <QtWidgets/QGraphicsItem>
#include <QtGui/QFont>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class LineChartDataToolTip : public QGraphicsItem
{
public:
    explicit LineChartDataToolTip(QChart* parent);
    // LineChartDataToolTip(LineChartDataToolTip&& m) : m_text(m.m_text), m_textRect(m.m_textRect), m_anchor(m.m_anchor), m_font(m.m_font), m_chart(m.m_chart) {}
    void setText(const QString& text);
    void setAnchor(QPointF point);
    void updateGeometry();
    void set_datapoint_identifier(int id);
    int get_datapoint_identifier() const;

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    QString m_text;
    QRectF m_textRect;
    QRectF m_rect;
    QPointF m_anchor;
    QFont m_font;
    QChart *m_chart;
    int m_data_point_ident;
};
