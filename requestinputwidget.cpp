#include "requestinputwidget.h"
#include "ui_requestinputwidget.h"

RequestInputWidget::RequestInputWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RequestInputWidget)
{
    ui->setupUi(this);
}

RequestInputWidget::~RequestInputWidget()
{
    delete ui;
}
