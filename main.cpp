#include "mainwindow.h"
#include "mdbconnection.h"

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <mongocxx/options/find.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

#include <QApplication>
#include <QDate>

// System headers. These can be removed, as pre compiled headers are used in CMakeLists.txt
#include <unordered_map>
#include <set>
#include <memory>
#include <string>
#include <iostream>
#include <optional>

// #include <instrumentation.h>


int main(int argc, char *argv[])
{
    { auto _ = teams_map(); auto _2 = teams_division_map(); } // Explicit init.
    QApplication a(argc, argv);
    MainWindow w;
    {
        auto ptr = std::make_unique<MDbConnection>("foo", "nhltest");
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
