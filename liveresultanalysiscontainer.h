#ifndef LIVERESULTANALYSISCONTAINER_H
#define LIVERESULTANALYSISCONTAINER_H

#include <QWidget>

namespace Ui {
class LiveResultAnalysisContainer;
}

class LiveResultAnalysisContainer : public QWidget
{
    Q_OBJECT

public:
    explicit LiveResultAnalysisContainer(QWidget *parent = nullptr);
    ~LiveResultAnalysisContainer();

private:
    Ui::LiveResultAnalysisContainer *ui;
};

#endif // LIVERESULTANALYSISCONTAINER_H
