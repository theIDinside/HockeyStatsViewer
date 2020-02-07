#pragma once
#include "mainwindow.h"
#include <QWidget>

namespace Ui {
class RequestInputWidget;
}

class RequestInputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RequestInputWidget(QWidget *parent = nullptr);
    ~RequestInputWidget();
    void register_mainwindow_connection(MainWindow& window);

private slots:
    void on_btnRequestStats_clicked();

signals:
    void signal_standing_analysis(int homescore, int awayscore, int period, int minutes);


private:
    Ui::RequestInputWidget *ui;
};
