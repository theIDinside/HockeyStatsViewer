#ifndef REQUESTINPUTWIDGET_H
#define REQUESTINPUTWIDGET_H

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

private:
    Ui::RequestInputWidget *ui;
};

#endif // REQUESTINPUTWIDGET_H
