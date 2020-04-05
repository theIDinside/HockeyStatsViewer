#include "mainwindow.h"
#include <QApplication>

#include "db/DB.h"

// #include <QDate>
// #include <instrumentation.h>
int main(int argc, char *argv[])
{
    { auto _ = teams_map(); auto _2 = teams_division_map(); } // Explicit init.
    QApplication a(argc, argv);
    MainWindow w;
    {
        auto ptr = std::make_unique<DB>("foo", "nhltest");
        auto game = ptr->get_game(2019020457);
        auto hTeam = game.home_team();
        auto aTeam = game.away_team();

        auto shotsHome = game.shots_by(game.get_team_type(hTeam));
        auto shotsAway = game.shots_by(game.get_team_type(aTeam));
        std::cout << "Shots home: " << shotsHome << std::endl;
        std::cout << "Shots away: " << shotsAway << std::endl;
        w.hook_db_connection(std::move(ptr));
    }
    w.show();
    return a.exec();
}
