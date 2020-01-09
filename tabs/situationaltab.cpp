#include "situationaltab.h"
SituationalTab::SituationalTab(QWidget* parent) : TeamStatsTab(parent)
{

}

void SituationalTab::update_chart_data(const TeamStats &home, const TeamStats &away)
{
    qDebug() << "Update situational data for teams";
    auto homeTeam = home.team_name();
    auto awayTeam = away.team_name();
    auto isHomePred = [&](const GameModel& g){ return g.home_team() == homeTeam; };
    auto isAwayPred = [&](const GameModel& g){ return g.away_team() == awayTeam; };


}
