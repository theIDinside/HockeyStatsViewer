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
    
    auto d1 = CalendarDate{9, 11, 2019};
    auto d2 = CalendarDate{10, 11, 2019};
    auto d3 = CalendarDate{11, 11, 2019};
    auto games_2019_11_09 = IMDB->get_games_at_date(d1);
    auto games_2019_11_10 = IMDB->get_games_at_date(d2);
    auto games_2019_11_11 = IMDB->get_games_at_date(d3);


    if(games_2019_11_09)
        std::cout << "Games at " << d1 << ": " << games_2019_11_09.value().size() << '\n';
    else 
        std::cout << "Found no games at " << d1 << '\n';

    if(games_2019_11_10)
        std::cout << "Games at " << d2 << ": " << games_2019_11_10.value().size() << '\n';
    else 
        std::cout << "Found no games at " << d2 << '\n';

    if(games_2019_11_11)
        std::cout << "Games at " << d3 << ": " << games_2019_11_11.value().size() << '\n';
    else 
        std::cout << "Found no games at " << d3 << '\n';

    auto hTeam = game.home_team();
    auto aTeam = game.away_team();


    auto shotsHome = game.shots_by(game.get_team_type(hTeam));
    auto shotsAway = game.shots_by(game.get_team_type(aTeam));
    std::cout << "Shots by " << game.m_teams.home << ": " << shotsHome << std::endl;
    std::cout << "Shots by: " << game.m_teams.away << ": "  << shotsAway << std::endl;
    // w.show();
    // return a.exec();
}
