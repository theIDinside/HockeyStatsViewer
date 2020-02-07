#include "liveresultanalysiscontainer.h"
#include "ui_liveresultanalysiscontainer.h"

LiveResultAnalysisContainer::LiveResultAnalysisContainer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiveResultAnalysisContainer)
{
    ui->setupUi(this);
}

LiveResultAnalysisContainer::~LiveResultAnalysisContainer()
{
    delete ui;
}
