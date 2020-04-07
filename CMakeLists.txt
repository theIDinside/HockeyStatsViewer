cmake_minimum_required(VERSION 3.16)
project(hsviewer)
set(EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -O3")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_COMPILER /usr/bin/clang++-8)
message("Hello world")
message("What is in this variable: ${CMAKE_MODULE_PATH}")
set(CMAKE_CXX_FLAGS )
set(Qt5_DIR /home/cx/apps/Qt/5.14.1/gcc_64/lib/cmake/Qt5/)
find_package(Qt5 COMPONENTS Widgets Core Gui Charts REQUIRED)


set(SOURCES_WITH_FORMS mainwindow.ui mainwindow.cpp gameboxeschart.ui gameboxeschart.cpp requestinputwidget.ui requestinputwidget.cpp gamedatapopup.ui gamedatapopup.cpp)
set(CHART_SOURCES charts/areachart.cpp charts/gameresultbox.cpp charts/linechart.cpp charts/linechartdatatooltip.cpp charts/piechart.cpp)
set(DATA_SOURCES data/gameinfomodel.cpp data/gamemodel.cpp data/popuptablemodel.cpp data/scoringmodel.cpp data/standing.cpp data/statsheet.cpp data/stattable.cpp data/trend.cpp data/gamestatistics.cpp db/DB.cpp db/DB.h data/CalendarDate.cpp db/types/GameResults.cpp valueholder.h db/types/Team.cpp db/types/Team.h)
set(TAB_SOURCES tabs/divisiontab.cpp tabs/goalstab.cpp tabs/lastfivegamestab.cpp tabs/liveresulttab.cpp tabs/periodtab.cpp tabs/resultstatstab.cpp tabs/seasontab.cpp tabs/situationaltab.cpp tabs/specialteamtab.cpp tabs/teamstatstab.cpp)
add_subdirectory(./dep/instrumentation)
add_subdirectory(./dep/json)

add_executable(hsviewer
    ${SOURCES_WITH_FORMS}
    main.cpp
    gamelistitemmodel.cpp
    gametime.cpp
    liveresultanalysiscontainer.cpp
    seasontablemodel.cpp
        teamstats.cpp
    utils.cpp
    ${CHART_SOURCES}
    ${DATA_SOURCES}
    ${TAB_SOURCES}
    utils.cpp
    iterators.h
    valueholder.h)

include_directories(".")

target_include_directories(hsviewer PRIVATE ~/dev/projects/HockeyStatsViewer/)
# target_link_libraries(hsviewer lpthread)
target_link_libraries(hsviewer nlohmann_json::nlohmann_json)
target_link_libraries(hsviewer Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Charts)
target_link_libraries(hsviewer instrumentation)
target_precompile_headers(hsviewer PUBLIC <string> <memory> <vector> <iostream> 
    <chrono> <ctime> <set> <unordered_map> <array> 
    <algorithm> <numeric> <optional> <cmath> 
    <QGraphicsView> <QObject> <QWidget> <QChart> <QChartView> <QLineSeries> <QAreaSeries> <QString>)
