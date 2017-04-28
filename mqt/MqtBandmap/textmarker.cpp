#include "textmarker.h"
#include <QDebug>

TextMarker::TextMarker(int x, int y, QString callsign)
{
    xCoord = x;
    yCoord = y;
    callSign = callsign;
    setAcceptHoverEvents(true);

}

void TextMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawText(xCoord, yCoord, callSign);
    //painter->drawRect(0,0,40,10);
}

QRectF TextMarker::boundingRect() const
{
    qreal penWidth = 3;
//    return QRectF(-10 - penWidth/2, -10 - penWidth/2, 20 + penWidth/2, 20 + penWidth/2);
    return QRectF(80 - penWidth/2, yCoord - penWidth/2 - 10,40,10);

}


void TextMarker::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{

   setToolTip("10:30, 360 deg");

}


void TextMarker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    int a =0;
    qDebug() << x() << " " << y();
    QGraphicsItem::mousePressEvent(event);
}
