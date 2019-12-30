#include "mainwindow.h"
#include "mdbconnection.h"
#include <iostream>
#include <optional>

#include <string>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <mongocxx/options/find.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <iostream>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

#include <QApplication>
#include <QDate>

#include <unordered_map>
#include <set>
#include <memory>


int main(int argc, char *argv[])
{
    { auto _ = teams_map(); auto _2 = teams_division_map(); } // Explicit init.
    QApplication a(argc, argv);
    MainWindow w;
    {
        auto ptr = std::make_unique<MDbConnection>("foo", "nhltest");
        w.hook_db_connection(std::move(ptr));
    }
    w.show();
    return a.exec();
}
