#pragma once
#include "mainwindow.h"
#include "teamstats.h"

#include <QWidget>
#include <QGraphicsScene>

#include <charts/gameresultbox.h>

namespace Ui {
class GameBoxesChart;
}

class GameBoxesChart : public QWidget
{
    Q_OBJECT
public:
    explicit GameBoxesChart(QWidget *parent = nullptr);
    ~GameBoxesChart();
    void update_data(const TeamStats& ts);
    void register_connections(MainWindow& window);
private:
    Ui::GameBoxesChart *ui;
    QString team;
    QGraphicsItem* teamNameItem;
    QGraphicsScene* scene;
    QList<GameResultBox*> boxes;
};
