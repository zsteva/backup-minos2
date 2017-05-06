#include "overlapwidget.h"
#include <QRect>
#include <QtWidgets>

OverLapWidget::OverLapWidget(QWidget *parent) : QWidget(parent)
{
    overlapOn = false;
}


void OverLapWidget::paintEvent(QPaintEvent *)
{
    QRect rect(6,0,80,20);
    QPainter painter(this);

    painter.setPen(Qt::black);

    if (overlapOn)
    {
        painter.fillRect(rect,Qt::red);
    }
    else
    {
        painter.fillRect(rect,Qt::lightGray);
    }
    painter.drawRect(rect);

}


void OverLapWidget::overlapDisplayUpdate(bool overlapStatus)
{
    if (overlapStatus != overlapOn)
    {
        overlapOn = overlapStatus;
        update();
    }

}

// This does nothing....just to manage the setText call...
void OverLapWidget::setText(QString text)
{
    QString s = text;
}
