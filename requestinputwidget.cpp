#include "requestinputwidget.h"
#include "ui_requestinputwidget.h"

#include <data/standing.h>

#include <tabs/liveresulttab.h>

RequestInputWidget::RequestInputWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RequestInputWidget)
{
    ui->setupUi(this);
    ui->awayScoreInput->setRange(0, 50);
    ui->homeScoreInput->setRange(0, 50);
    ui->periodInput->setRange(1, 4);
    ui->timeInput->setRange(0, 19);
}

RequestInputWidget::~RequestInputWidget()
{
    delete ui;
}

void RequestInputWidget::register_mainwindow_connection(MainWindow &window)
{
    connect(this, &RequestInputWidget::signal_standing_analysis, &window, &MainWindow::live_result_analysis_requested);
}

void RequestInputWidget::on_btnRequestStats_clicked()
{
    int awayScore = ui->awayScoreInput->value();
    int homeScore = ui->homeScoreInput->value();
    int period = ui->periodInput->value();
    int periodMinutes = ui->timeInput->value();
    emit signal_standing_analysis(homeScore, awayScore, period, periodMinutes); // Let mainwindow take care of the rest
}
