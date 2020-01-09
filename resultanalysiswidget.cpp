#include "resultanalysiswidget.h"
#include "ui_resultanalysiswidget.h"

StatRequestInputWidget::StatRequestInputWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatRequestInputWidget)
{
    ui->setupUi(this);
}

StatRequestInputWidget::~StatRequestInputWidget()
{
    delete ui;
}
