cmake_minimum_required(VERSION 3.16)
project(HSViewer)
set(EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -O3")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_COMPILER /usr/bin/clang++-8)
message("What is in this variable: ${CMALE_MODULE_PATH}")
set(CMAKE_CXX_FLAGS )
find_package(Qt5 COMPONENTS Widgets Core Gui Charts REQUIRED)
set(Qt5_DIR /home/cx/apps/Qt/5.12.6/gcc_64/lib/cmake/Qt5/)


set(SOURCES_WITH_FORMS mainwindow.ui mainwindow.cpp gameboxeschart.ui gameboxeschart.cpp requestinputwidget.ui requestinputwidget.cpp gamedatapopup.ui gamedatapopup.cpp)
set(CHART_SOURCES charts/areachart.cpp charts/gameresultbox.cpp charts/linechart.cpp charts/linechartdatatooltip.cpp charts/piechart.cpp)
set(DATA_SOURCES data/gameinfomodel.cpp data/gamemodel.cpp data/popuptablemodel.cpp data/scoringmodel.cpp data/standing.cpp data/statsheet.cpp data/stattable.cpp data/trend.cpp data/gamestatistics.cpp)
set(TAB_SOURCES tabs/divisiontab.cpp tabs/goalstab.cpp tabs/lastfivegamestab.cpp tabs/liveresulttab.cpp tabs/periodtab.cpp tabs/resultstatstab.cpp tabs/seasontab.cpp tabs/situationaltab.cpp tabs/specialteamtab.cpp tabs/teamstatstab.cpp)
add_subdirectory(./dep/instrumentation)

include_directories(/usr/local/include/mongocxx/v_noabi /usr/local/include/bsoncxx/v_noabi)

add_executable(HSViewer
    ${SOURCES_WITH_FORMS}
    main.cpp
    gamelistitemmodel.cpp
    gametime.cpp
    liveresultanalysiscontainer.cpp
    mdbconnection.cpp
    seasontablemodel.cpp
    team.cpp
    teamstats.cpp
    utils.cpp
    ${CHART_SOURCES}
    ${DATA_SOURCES}
    ${TAB_SOURCES}
    utils.cpp
    iterators.h
    valueholder.h)

target_include_directories(HSViewer PRIVATE ~/dev/projects/HockeyStatsViewer/)
target_link_libraries(HSViewer -L/usr/local/lib -lmongocxx -lbsoncxx -lpthread)
target_link_libraries(HSViewer Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Charts)
target_link_libraries(HSViewer instrumentation)
target_precompile_headers(HSViewer PUBLIC <string> <memory> <vector> <iostream> <chrono> <ctime> <set> <unordered_map> <array> <algorithm> <numeric> <optional> <tuple> <cmath>)