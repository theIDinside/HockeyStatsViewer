#pragma once
#include <QGraphicsItem>
#include <QFont>
#include <QPainter>
#include <QObject>

class GameResultBox;
class SignalsHandler : public QObject {
    Q_OBJECT
public:
    SignalsHandler(GameResultBox* box, QObject* parent=nullptr);
    ~SignalsHandler() {}
    void signal();
    GameResultBox* handledObject;
signals:
    void gameDataPopUp(int id);
};


class GameResultBox : public QGraphicsItem
{
    // Q_OBJECT
public:
    GameResultBox();

    // QGraphicsItem interface
public:
    friend class SignalsHandler;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void set_won(bool won=true);
    QPointF connect_point_right() const;
    QPointF connect_point_left() const;
    void set_text(QString text);
    void set_gameid(int gameID);
    friend QLineF connecting_line(const GameResultBox& a, const GameResultBox& b);

    SignalsHandler sig_handle;
    const SignalsHandler* get_handle();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    int gameID;
    QString character;
    bool gameWon;
    QRectF boundingBoxText;
    QFont m_font;
};



QLineF connecting_line(const GameResultBox& a, const GameResultBox& b);
