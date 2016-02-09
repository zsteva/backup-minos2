#include "minossplitter.h"
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QMenu>
//============================================================================
MinosSplitterHandle::MinosSplitterHandle(Qt::Orientation orientation, QSplitter *parent)
    : QSplitterHandle(orientation, parent)
{
    gradient.setColorAt(0.0, Qt::darkGreen);
    gradient.setColorAt(0.25, Qt::white);
    gradient.setColorAt(1.0, Qt::darkGreen);

}

void MinosSplitterHandle::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (orientation() == Qt::Horizontal) {
        gradient.setStart(rect().left(), rect().height()/2);
        gradient.setFinalStop(rect().right(), rect().height()/2);
        painter.fillRect(event->rect(), QBrush(gradient));

    } else {
        gradient.setStart(rect().width()/2, rect().top());
        gradient.setFinalStop(rect().width()/2, rect().bottom());
        painter.fillRect(event->rect(), QBrush(gradient));
    }
}
//============================================================================
MinosSplitter::MinosSplitter(QWidget *parent)
    : QSplitter(parent), myHandle(0)
{
}
QSplitterHandle *MinosSplitter::createHandle()
{
    myHandle = new MinosSplitterHandle(orientation(), this);
    return myHandle;
}
