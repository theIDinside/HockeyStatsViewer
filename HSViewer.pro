    #-------------------------------------------------
#
# Project created by QtCreator 2019-11-26T23:07:33
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HSViewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

PRECOMPILED_HEADER = PCH.h

CONFIG += c++17 precompile_header

SOURCES += \
    charts/areachart.cpp \
    charts/gameresultbox.cpp \
    charts/linechart.cpp \
    charts/linechartdatatooltip.cpp \
    charts/piechart.cpp \
    data/gameinfomodel.cpp \
    data/gamemodel.cpp \
    data/gamestatistics.cpp \
    data/popuptablemodel.cpp \
    data/scoringmodel.cpp \
    data/standing.cpp \
    data/statsheet.cpp \
    data/stattable.cpp \
    data/trend.cpp \
    gameboxeschart.cpp \
    gamedatapopup.cpp \
    liveresultanalysiscontainer.cpp \
        main.cpp \
        mainwindow.cpp \
    gametime.cpp \
    gamelistitemmodel.cpp \
    requestinputwidget.cpp \
    seasontablemodel.cpp \
    tabs/divisiontab.cpp \
    tabs/goalstab.cpp \
    tabs/lastfivegamestab.cpp \
    tabs/liveresulttab.cpp \
    tabs/periodtab.cpp \
    tabs/resultstatstab.cpp \
    tabs/seasontab.cpp \
    tabs/situationaltab.cpp \
    tabs/specialteamtab.cpp \
    tabs/teamstatstab.cpp \
    teamstats.cpp \
    team.cpp \
    utils.cpp

HEADERS += \
    PCH.h \
    charts/areachart.h \
    charts/gameresultbox.h \
    charts/linechart.h \
    charts/linechartdatatooltip.h \
    charts/piechart.h \
    data/gameinfomodel.h \
    data/gamemodel.h \
    data/gamestatistics.h \
    data/popuptablemodel.h \
    data/scoringmodel.h \
    data/standing.h \
    data/statsheet.h \
    data/stattable.h \
    data/trend.h \
    gameboxeschart.h \
    gamedatapopup.h \
    liveresultanalysiscontainer.h \
        mainwindow.h \
    gametime.h \
    requestinputwidget.h \
    seasontablemodel.h \
    tabs/divisiontab.h \
    tabs/goalstab.h \
    tabs/lastfivegamestab.h \
    tabs/liveresulttab.h \
    tabs/periodtab.h \
    tabs/resultstatstab.h \
    tabs/seasontab.h \
    tabs/situationaltab.h \
    tabs/specialteamtab.h \
    tabs/teamstatstab.h \
    utils.h \
    valueholder.h \
    gamelistitemmodel.h \
    teamstats.h \
    team.h \
    iterators.h

FORMS += \
        gameboxeschart.ui \
        gamedatapopup.ui \
        liveresultanalysiscontainer.ui \
        mainwindow.ui \
        requestinputwidget.ui

INCLUDEPATH += /home/cx/dev/projects/HockeyStatsViewer/dep/json/include


LIBS += -L/usr/local/lib

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
    message("USING PRE COMPILED HEADERS!")
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    CMakeLists.txt
