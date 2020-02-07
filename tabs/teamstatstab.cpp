#include "teamstatstab.h"
TeamStatsTab::TeamStatsTab(QWidget *parent) : QWidget(parent)
{

}

void TeamStatsTab::hide_series(SeriesType type)
{
    hide_series_impl(type);
}

void TeamStatsTab::show_series(SeriesType type)
{
    show_series_impl(type);
}
