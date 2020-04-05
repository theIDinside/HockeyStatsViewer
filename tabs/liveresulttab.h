#pragma once
#include "teamstatstab.h"

#include <QGridLayout>
#include <QLabel>
#include <QObject>
#include <requestinputwidget.h>
#include <charts/areachart.h>
#include <data/standing.h>

/* Tab that takes input of current live standing in a game and returns some analytic data from previous games with similar-ish results */
class LiveResultTab : public TeamStatsTab
{
    Q_OBJECT
public:
    explicit LiveResultTab(QWidget* parent=nullptr);
    // ~LiveResultTab();

    void register_inputs_with_main(MainWindow& window);

private:
    RequestInputWidget* mInputWidget;
    QGridLayout* mRightPaneLayout;
    QScrollArea* mContentScrollArea;
    QHBoxLayout* mMainLayout;
    QVBoxLayout* mLeftPaneLayout;

    LineChart* mScoreDistOver60Home;
    LineChart* mLetupDistOver60Home;

    LineChart* mScoreDistOver60Away;
    LineChart* mLetupDistOver60Away;
    AreaChart* goalDist;
    QScrollArea* answersScrollArea;
    QList<QLabel*> mAnswers;
public:
    void add_answer(const std::string& text);

    // TeamStatsTab interface
public:
    void set_chart_title_string_prefix(QString string) override;

public slots:
    void update_chart_data(const TeamStats &home, const TeamStats &away) override;
    void analyze_with_standing(const TeamStats& home, const TeamStats& away, Standing standing);

    // TeamStatsTab interface
private:
    void hide_series_impl(SeriesType SType) override;
    void show_series_impl(SeriesType SType) override;
};
