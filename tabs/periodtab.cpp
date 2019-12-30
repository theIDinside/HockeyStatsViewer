#include "periodtab.h"

PeriodTab::PeriodTab(QWidget* parent) : TeamStatsTab(parent), m_layout{this}
{
    h_chart_gf = new LineChart("Goals for");
    h_chart_ga = new LineChart("Goals Against");
    h_chart_sf = new LineChart("Shots for");
    h_chart_sa = new LineChart("Shots against");
    a_chart_gf = new LineChart("Goals for");
    a_chart_ga = new LineChart("Goals Against");
    a_chart_sf = new LineChart("Shots for");
    a_chart_sa = new LineChart("Shots against");

    m_layout.addWidget(h_chart_gf, 0, 0);   m_layout.addWidget(a_chart_gf, 0, 1);
    m_layout.addWidget(h_chart_ga, 1, 0);   m_layout.addWidget(a_chart_ga, 1, 1);
    m_layout.addWidget(h_chart_sf, 2, 0);   m_layout.addWidget(a_chart_sf, 2, 1);
    m_layout.addWidget(h_chart_sa, 3, 0);   m_layout.addWidget(a_chart_sa, 3, 1);

    chartPtrs << h_chart_gf << h_chart_ga << h_chart_sf << h_chart_sa << a_chart_gf << a_chart_ga << a_chart_sf << a_chart_sa;
}

PeriodTab::~PeriodTab()
{
    for(auto& ptr : chartPtrs) delete ptr;
}

void PeriodTab::update_chart_data(const TeamStats &home, const TeamStats &away)
{
    using PeriodsSeries = std::vector<std::vector<double>>;

    auto homeTeam = home.team_name();
    auto awayTeam = away.team_name();

    h_chart_gf->set_title("Goals for/Period average by " + homeTeam);
    h_chart_ga->set_title("Goals against/Period average by " + homeTeam);
    h_chart_sf->set_title("Shots for/Period average by " + homeTeam);
    h_chart_sa->set_title("Shots against/Period average by " + homeTeam);

    a_chart_gf->set_title("Goals for/Period average by " + awayTeam);
    a_chart_ga->set_title("Goals against/Period average by " + awayTeam);
    a_chart_sf->set_title("Shots for/Period average by " + awayTeam);
    a_chart_sa->set_title("Shots against/Period average by " + awayTeam);


    std::vector<GamePeriod> periodsToIterate{GamePeriod::First, GamePeriod::Second, GamePeriod::Third};
    std::vector<std::string> periods{"1", "2", "3"};

    PeriodsSeries hGFAvg{};
    PeriodsSeries hGAAvg{};
    PeriodsSeries hSFAvg{};
    PeriodsSeries hSAAvg{};

    PeriodsSeries aGFAvg{};
    PeriodsSeries aGAAvg{};
    PeriodsSeries aSFAvg{};
    PeriodsSeries aSAAvg{};

    for(const auto& period : periodsToIterate) {
        auto h_gfpAverage = home.gf_avg_by_period(TeamStats::Span::Five, period);
        auto h_gapAverage = home.ga_avg_by_period(TeamStats::Span::Five, period);
        auto h_sfpAverage = home.sf_avg_by_period(TeamStats::Span::Five, period);
        auto h_sapAverage = home.sa_avg_by_period(TeamStats::Span::Five, period);
        hGFAvg.push_back(h_gfpAverage);
        hGAAvg.push_back(h_gapAverage);
        hSFAvg.push_back(h_sfpAverage);
        hSAAvg.push_back(h_sapAverage);

        auto a_gfpAverage = away.gf_avg_by_period(TeamStats::Span::Five, period);
        auto a_gapAverage = away.ga_avg_by_period(TeamStats::Span::Five, period);
        auto a_sfpAverage = away.sf_avg_by_period(TeamStats::Span::Five, period);
        auto a_sapAverage = away.sa_avg_by_period(TeamStats::Span::Five, period);
        aGFAvg.push_back(a_gfpAverage);
        aGAAvg.push_back(a_gapAverage);
        aSFAvg.push_back(a_sfpAverage);
        aSAAvg.push_back(a_sapAverage);
    }
    h_chart_gf->clear_and_update_new_multi_series_data(periods, hGFAvg);
    h_chart_ga->clear_and_update_new_multi_series_data(periods, hGAAvg);
    h_chart_sf->clear_and_update_new_multi_series_data(periods, hSFAvg);
    h_chart_sa->clear_and_update_new_multi_series_data(periods, hSAAvg);

    a_chart_gf->clear_and_update_new_multi_series_data(periods, aGFAvg);
    a_chart_ga->clear_and_update_new_multi_series_data(periods, aGAAvg);
    a_chart_sf->clear_and_update_new_multi_series_data(periods, aSFAvg);
    a_chart_sa->clear_and_update_new_multi_series_data(periods, aSAAvg);
}
