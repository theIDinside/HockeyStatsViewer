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
    std::unique_ptr<DB> IMDB = nullptr;
    {

        auto imdb_opt = DB::init_database("./assets/db");
        if(!imdb_opt) {
            std::cout << "Failed to initialize in-memory database. Exiting." << std::endl;
            std::abort();
        }
        IMDB = std::move(imdb_opt.value());
        w.hook_db_connection(std::move(IMDB));
    }
    w.show();
    return a.exec();
}
