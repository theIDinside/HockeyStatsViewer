#include "linechart.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <QValueAxis>

#include <unordered_map>
#include <string>

static std::unordered_map<std::string, std::string> teams{};

std::string abbreviate_team_name(const std::string& name) {
    if(teams.size() == 0) {
        teams["Anaheim Ducks"]          = "ANA";
        teams["Boston Bruins"]          = "BOS";
        teams["Arizona Coyotes"]        = "ARI";
        teams["Buffalo Sabres"]         = "BUF";
        teams["Carolina Hurricanes"]    = "CAR";
        teams["Chicago Blackhawks"]     = "CHI";
        teams["Calgary Flames"]         = "CAL";
        teams["Columbus Blue Jackets"]  = "CBJ";
        teams["Colorado Avalanche"]     = "COL";
        teams["Dallas Stars"]           = "DAL";
        teams["Edmonton Oilers"]        = "EDM";
        teams["New Jersey Devils"]      = "NJD";
        teams["New York Islanders"]     = "NYI";
        teams["New York Rangers"]       = "NYR";
        teams["Minnesota Wild"]         = "MIN";
        teams["Philadelphia Flyers"]    = "PHI";
        teams["Detroit Red Wings"]      = "DET";
        teams["Los Angeles Kings"]      = "LAK";
        teams["Ottawa Senators"]        = "OTT";
        teams["Montreal Canadiens"]     = "MTL";
        teams["Pittsburgh Penguins"]    = "PIT";
        teams["Washington Capitals"]    = "WSH";
        teams["St. Louis Blues"]        = "STL";
        teams["San Jose Sharks"]        = "SJS";
        teams["Tampa Bay Lightning"]    = "TBL";
        teams["Vancouver Canucks"]      = "VAN";
        teams["Toronto Maple Leafs"]    = "TOR";
        teams["Winnipeg Jets"]          = "WPG";
        teams["Vegas Golden Knights"]   = "VGK";
        teams["Florida Panthers"]       = "FLO";
        teams["Nashville Predators"]    = "NSH";
    }
    return teams[name];
}


LineChart::LineChart(std::string title, QWidget* parent) : QGraphicsView(new QGraphicsScene, parent), m_title(std::move(title)), m_chart(new QChart), m_tooltip(nullptr), m_series_data_home{}, m_series_data_away{}
{
    setDragMode(QGraphicsView::NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_chart->setTitle(m_title.c_str());
    m_chart->createDefaultAxes();
    m_chart->setAnimationOptions(QChart::AllAnimations);
    m_chart->setAnimationDuration(350);
    scene()->addItem(m_chart);
    this->setMouseTracking(true);
    this->setMinimumHeight(275);
}

void LineChart::clear_and_update_new_data(const std::string &home, const std::string &away, std::vector<double> home_chart_data, std::vector<double> away_chart_data, LineChartRangeType type)
{
    assert(home_chart_data.size() == away_chart_data.size() && "Both vectors of data must be of equal length");
    valueType = type;
    m_home_team = home.c_str();
    m_away_team = away.c_str();
    m_series_data_away.clear();
    m_series_data_home.clear();
    m_series.clear();
    m_chart->removeAllSeries();
    auto tooltips = m_saved_tooltips;
    for(auto ptr : tooltips) {
        delete ptr;
    }
    m_saved_tooltips.clear();
    std::copy(home_chart_data.cbegin(), home_chart_data.cend(), std::back_inserter(m_series_data_home));
    std::copy(away_chart_data.cbegin(), away_chart_data.cend(), std::back_inserter(m_series_data_away));

    // Tell the GraphicsView to start redrawing etc
    QLineSeries *home_series = new QLineSeries;
    QLineSeries *away_series = new QLineSeries;

    QPen ph(QColor(0,0,255));
    ph.setWidthF(2.5);
    home_series->setPen(ph);

    QPen pa(QColor(0, 255, 0));
    pa.setWidthF(2.5);
    away_series->setPen(pa);
    auto max_value = 0.0;
    auto min_value = 100.0;
    if(type == LineChartRangeType::Percent) min_value = 0;
    auto stepix = 0.0;
    for(const auto& val : home_chart_data) {
        home_series->append(stepix, val);
        stepix += 1;
        max_value = std::max(val, max_value);
        min_value = std::min(val, min_value);
    }
    stepix = 0;
    for(const auto& val : away_chart_data) {
        away_series->append(stepix, val);
        stepix += 1;
        max_value = std::max(val, max_value);
        min_value = std::min(val, min_value);
    }
    if(min_value >= 1.0) min_value-=0.3;
    max_value += 0.3;
    if(type == LineChartRangeType::Percent) {
        max_value = (std::max(max_value, 50.0) > 50.0) ? 100.0 : 50.0;
    }
    m_series << away_series;
    m_series << home_series;
    m_chart->addSeries(away_series);
    m_chart->addSeries(home_series);
    away_series->setName(m_away_team);
    home_series->setName(m_home_team);
    m_chart->createDefaultAxes();
    m_chart->axes(Qt::Horizontal).first()->setRange(0, (double)home_chart_data.size()-1);
    m_chart->axes(Qt::Vertical).first()->setRange(min_value, max_value);
    m_chart->setAcceptHoverEvents(true);
    setRenderHint(QPainter::Antialiasing);
    connect(home_series, &QLineSeries::clicked, this, &LineChart::keepTooltip);
    connect(home_series, &QLineSeries::hovered, this, &LineChart::tooltip);
    connect(away_series, &QLineSeries::clicked, this, &LineChart::keepTooltip);
    connect(away_series, &QLineSeries::hovered, this, &LineChart::tooltip);
}

void LineChart::clear_and_update_new_multi_series_data(const std::vector<std::string> &series_names, std::vector<std::vector<double> > series_vector, LineChartRangeType type)
{
    assert(series_names.size() == series_vector.size() && "Series name vector must match amount of series, and vice versa");
    valueType = type;
    m_series.clear();
    multi_series.clear();
    m_chart->removeAllSeries();

    auto tooltips = m_saved_tooltips;
    for(auto ptr : tooltips) {
        delete ptr;
    }
    m_saved_tooltips.clear();
    int seriesIndex = 0;
    auto max_value = 0.0;
    auto min_value = 100.0;
    if(type == LineChartRangeType::Percent) min_value = 0;
    for(const auto& seriesTitle: series_names) {
        const auto& series = series_vector[seriesIndex];
        std::vector<double> res;
        std::copy(series.begin(), series.end(), std::back_inserter(res));
        multi_series.push_back(res);
        QLineSeries* lineSeries = new QLineSeries;
        // Tell the GraphicsView to start redrawing etc
        auto stepix = 0.0;
        for(const auto& val : series) {
            lineSeries->append(stepix, val);
            stepix += 1;
            max_value = std::max(val, max_value);
            min_value = std::min(val, min_value);
        }
        if(min_value >= 1.0) min_value-=0.3;
        max_value += 0.3;
        if(type == LineChartRangeType::Percent) {
            max_value = (std::max(max_value, 50.0) > 50.0) ? 100.0 : 50.0;
        }
        m_series << lineSeries;
        m_chart->addSeries(lineSeries);
        lineSeries->setName(QString(seriesTitle.c_str()));
        m_chart->createDefaultAxes();
        m_chart->axes(Qt::Horizontal).first()->setRange(0, (double)series.size()-1);
        m_chart->axes(Qt::Vertical).first()->setRange(min_value, max_value);
        m_chart->setAcceptHoverEvents(true);
        setRenderHint(QPainter::Antialiasing);
        connect(lineSeries, &QLineSeries::clicked, this, &LineChart::keepTooltip);
        connect(lineSeries, &QLineSeries::hovered, this, &LineChart::tooltip);
        seriesIndex++;
    }
}

void LineChart::set_title(const std::string &title)
{
    m_chart->setTitle(title.c_str());
}

void LineChart::set_title(const QString &title)
{
    m_chart->setTitle(title);
}

void LineChart::resizeEvent(QResizeEvent *event)
{
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
         m_chart->resize(event->size());
         const auto tooltips = m_saved_tooltips;
         for(auto& ptr : tooltips) {
             ptr->updateGeometry();
         }
    }
    QGraphicsView::resizeEvent(event);
}

void LineChart::mouseMoveEvent(QMouseEvent *event)
{
    // Do stuff when mouse is moved...
    QGraphicsView::mouseMoveEvent(event);
}

void LineChart::keepTooltip()
{
    m_saved_tooltips.append(std::move(m_tooltip));
    m_tooltip = new LineChartDataToolTip{m_chart};
}

std::size_t from_period_string(const std::string& p) {
    if(p == "1") return 0;
    else if(p == "2") return 1;
    else if(p == "3") return 2;
    else return 3;
}

void LineChart::tooltip(QPointF point, bool state)
{
    if (!m_tooltip)
        m_tooltip = new LineChartDataToolTip(m_chart);
    if (state) {
        auto sen = sender();
        QLineSeries* series = qobject_cast<QLineSeries*>(sen);
        if(series != nullptr) {
            auto name = series->name();
            if(name == m_home_team) {
                auto nearest_x = std::round(point.x());
                auto nearest_y = m_series_data_home[static_cast<std::size_t>(nearest_x)];
                auto actual_data_point = QPointF{nearest_x, nearest_y};
                auto valueTypeString = (valueType == LineChartRangeType::Value) ? "" : "%";
                auto alreadyShown = false;
                for(const auto& ttip : m_saved_tooltips) {
                    if(ttip->get_datapoint_identifier() == HOMEID+nearest_x) alreadyShown = true;
                }
                if(!alreadyShown) {
                    auto abb_name = abbreviate_team_name(name.toStdString());
                    m_tooltip->setText(QString("%1:\n %2%3").arg(abb_name.c_str()).arg(nearest_y).arg(valueTypeString));
                    m_tooltip->setAnchor(actual_data_point);
                    m_tooltip->setZValue(11);
                    m_tooltip->set_datapoint_identifier(HOMEID+nearest_x);
                    m_tooltip->updateGeometry();
                    m_tooltip->show();
                }
            } else if(name == m_away_team) {
                auto nearest_x = std::round(point.x());
                auto nearest_y = m_series_data_away[static_cast<std::size_t>(nearest_x)];
                auto actual_data_point = QPointF{nearest_x, nearest_y};
                auto valueTypeString = (valueType == LineChartRangeType::Value) ? "" : "%";
                auto alreadyShown = false;
                for(const auto& ttip : m_saved_tooltips) {
                    if(ttip->get_datapoint_identifier() == AWAYID+nearest_x) alreadyShown = true;
                }
                if(!alreadyShown) {
                    auto abb_name = abbreviate_team_name(name.toStdString());
                    m_tooltip->setText(QString("%1:\n %2%3").arg(abb_name.c_str()).arg(nearest_y).arg(valueTypeString));
                    m_tooltip->setAnchor(actual_data_point);
                    m_tooltip->setZValue(11);
                    m_tooltip->set_datapoint_identifier(AWAYID+nearest_x);
                    m_tooltip->updateGeometry();
                    m_tooltip->show();
                }
            } else if(name == "1" || name == "2" || name == "3") {
                std::cout << "Index is: " << name.toStdString() << std::endl;
                auto index = from_period_string(name.toStdString());
                auto nearest_x = std::round(point.x());
                auto nearest_y = multi_series[index][static_cast<std::size_t>(nearest_x)];
                auto actual_data_point = QPointF{nearest_x, nearest_y};
                auto valueTypeString = (valueType == LineChartRangeType::Value) ? "" : "%";
                auto alreadyShown = false;
                for(const auto& ttip : m_saved_tooltips) {
                    if(ttip->get_datapoint_identifier() == (index*100)+nearest_x) alreadyShown = true;
                }
                if(!alreadyShown) {
                    m_tooltip->setText(QString("%1:\n %2%3").arg(name).arg(nearest_y).arg(valueTypeString));
                    m_tooltip->setAnchor(actual_data_point);
                    m_tooltip->setZValue(11);
                    m_tooltip->set_datapoint_identifier((index*100)+nearest_x);
                    m_tooltip->updateGeometry();
                    m_tooltip->show();
                }
            } else {
                std::cout << "error. unknown series index" << std::endl;
            }
        }
    } else {
        m_tooltip->hide();
    }
}
