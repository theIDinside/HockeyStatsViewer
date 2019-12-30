#include "gameresultbox.h"
#include <QBrush>
#include <QDebug>

QLineF connecting_line(const GameResultBox& a, const GameResultBox& b) {
    return QLineF{a.connect_point_right(), b.connect_point_left()};
}

GameResultBox::GameResultBox() : m_font{}, sig_handle{this}
{
    setFlag(ItemIsMovable);
    m_font.setPointSize(25);
    QFontMetrics metrics(m_font);
    m_font.setPointSize(25);
    m_font.setBold(true);
    character = "W";
    boundingBoxText = metrics.boundingRect(QRect{0,0,50,50}, Qt::AlignCenter, character);
}

void GameResultBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    auto bounds = boundingRect();
    QBrush brush{Qt::blue}; // if box shows blue color, we know we have not initialized this QGraphicsItem with it's proper Game Data.

    if(gameWon) {
        // set to green
        brush.setColor(Qt::green);
    } else {
        // set to red
        brush.setColor(Qt::red);
    }
    painter->setFont(m_font);
    painter->fillRect(bounds, brush);
    painter->drawRect(bounds);
    painter->drawText(boundingBoxText, character);
}

QRectF GameResultBox::boundingRect() const {
    return QRect{0,0, 50, 50};
}

void GameResultBox::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    qDebug() << "Game ID: " << gameID;
    sig_handle.signal();
    QGraphicsItem::mousePressEvent(event);
}

void GameResultBox::set_text(QString text) {
    character = text;
    m_font.setPointSize(25);
    QFontMetrics metrics(m_font);
    boundingBoxText = metrics.boundingRect(QRect{0,0,50,50}, Qt::AlignCenter, character);
    m_font.setPointSize(25);
    m_font.setBold(true);
    prepareGeometryChange();
}

void GameResultBox::set_gameid(int gameID)
{
    this->gameID = gameID;
}

const SignalsHandler *GameResultBox::get_handle()
{
    return &sig_handle;
}

void GameResultBox::set_won(bool won) {
    if(won) set_text("W");
    else set_text("L");
    gameWon = won;
}

QPointF GameResultBox::connect_point_right() const
{
    auto bounds = boundingRect();
    auto point = QPointF{pos().x() + bounds.width(), pos().y() + bounds.height() / 2};
    return point;
}

QPointF GameResultBox::connect_point_left() const
{
    auto bounds = boundingRect();
    auto point = QPointF{pos().x(), pos().y()+bounds.height()/2};
    return point;
}

SignalsHandler::SignalsHandler(GameResultBox* box, QObject *parent) : QObject(parent), handledObject{box}
{

}

void SignalsHandler::signal()
{
    emit gameDataPopUp(handledObject->gameID);
}
