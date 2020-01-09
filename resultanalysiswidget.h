#ifndef RESULTANALYSISWIDGET_H
#define RESULTANALYSISWIDGET_H

#include <QWidget>

namespace Ui {
class StatRequestInputWidget;
}

class StatRequestInputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatRequestInputWidget(QWidget *parent = nullptr);
    ~StatRequestInputWidget();

private:
    Ui::StatRequestInputWidget *ui;
};

#endif // RESULTANALYSISWIDGET_H
