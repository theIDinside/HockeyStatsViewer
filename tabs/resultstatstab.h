#pragma once
#include "teamstatstab.h"

class ResultStatsTab : public TeamStatsTab
{
    Q_OBJECT
public:
    ResultStatsTab(QWidget* parent=nullptr);

    // TeamStatsTab interface
public:
    void set_chart_title_string_prefix(QString string) override;
public slots:
    void update_chart_data(const TeamStats &home, const TeamStats &away) override;
};
