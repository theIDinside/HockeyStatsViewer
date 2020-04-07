#include "mainwindow.h"
#include <QApplication>

#include "db/DB.h"

// #include <QDate>
// #include <instrumentation.h>
int main(int argc, char *argv[])
{
    { auto _ = teams_map(); auto _2 = teams_division_map(); } // Explicit init.
    // QApplication a(argc, argv);
    // MainWindow w;
    std::unique_ptr<DB> IMDB = nullptr;
    {

        auto imdb_opt = DB::init_database("./assets/db");
        if(!imdb_opt) {
            std::cout << "Failed to initialize in-memory database. Panicking." << std::endl;
            std::abort();
        }
        IMDB = std::move(imdb_opt.value());
        // w.hook_db_connection(std::move(IMDB));
    }
    auto id = 2019020001;
    auto game = IMDB->get_game(id);
    auto hTeam = game.home_team();
    auto aTeam = game.away_team();


    auto shotsHome = game.shots_by(game.get_team_type(hTeam));
    auto shotsAway = game.shots_by(game.get_team_type(aTeam));
    std::cout << "Shots by " << game.m_teams.home << ": " << shotsHome << std::endl;
    std::cout << "Shots by: " << game.m_teams.away << ": "  << shotsAway << std::endl;
    // w.show();
    // return a.exec();
}
