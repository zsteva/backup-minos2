#include "freqdial.h"

FreqDial::FreqDial()
{

}




QRectF FreqDial::boundingRect() const
{
//    qreal penwidth = 1;
//    return QRectF(-10 - penwidth/2, -10 - penwidth/2, 20 + penwidth, 20 + penwidth );
//    return QRectF(0, 0, 100, dialData::MAXSCALEY );
      return QRectF(0, 0, 100, maxScaleY );
}



void FreqDial::changeBoundingRect(int height)
{
    prepareGeometryChange();
    setCurHeight(height);
    boundingRect();
}



void FreqDial::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{

// painter =  new QPainter( this );
    zoomLevel = 8;

    drawScale(painter, currentFreq, maxScaleY);
    drawCursor(painter, currentFreq);


}


void FreqDial::setCurFreq(double frequency)
{
    currentFreq = frequency;
}


double FreqDial::getCurFreq()
{
   return currentFreq;
}


void FreqDial::setCurHeight(int height)
{
    maxScaleY = height;
}

int FreqDial::getCurHeight()
{
    return maxScaleY;
}

void FreqDial::drawScale(QPainter *painter, double frequency, int scaleHeight)
{
    maxScaleY = scaleHeight;
    qint32 freq = 0;
    freq = frequency/1000;
    int freqRange = maxScaleY / dialData::khzPixelStep[zoomLevel];
    int mid_freqRange = freqRange/2;
    scaleStartFreq = freq - mid_freqRange;
    scaleEndFreq = scaleStartFreq + freqRange;

    //QRect scaleRec(0,0,70,dialData::MAXSCALEY);
    QRect scaleRec(0,0,70, maxScaleY);
    QBrush scaleBackGndBrush(Qt::lightGray, Qt::SolidPattern);

    QPen scalePen(Qt::black);
    scalePen.setWidth(2);
    painter->setPen(scalePen);

    painter->fillRect(scaleRec, scaleBackGndBrush);

    //painter->drawLine(QPoint(70,0),QPoint(70,dialData::MAXSCALEY));
    painter->drawLine(QPoint(70,0),QPoint(70,maxScaleY));

    QPen markerPen(Qt::blue);
    markerPen.setWidth(1);
    painter->setPen(markerPen);


    int markStep = dialData::khzStep[zoomLevel] * dialData::khzPixelStep[zoomLevel];
    int markCount = 0;
    int markFreq = scaleStartFreq;

    if (dialData::minorMarker[zoomLevel] == 0)
    {

        for (int ycoord = 0; ycoord < maxScaleY; ycoord += markStep)
        {
            painter->drawLine(QPoint(dialData::fMajMrkXStart, ycoord), QPoint(dialData::fMajMrkXEnd, ycoord));
            painter->drawText(QRect(0, ycoord - 7, 45, 12), Qt::AlignRight, QString::number(markFreq));
            if (dialData::khzStep[zoomLevel] == 1)
            {
                markFreq += 1;
            }
            else
            {
                markFreq += 50;
            }
        }

    }
    else if (dialData::minorMarker[zoomLevel] == 1)
    {

        for (int ycoord = 0; ycoord < maxScaleY; ycoord += markStep)
        {

            if (markCount == 0 || markCount % 5 == 0)
            {
                painter->drawLine(QPoint(dialData::fMajMrkXStart, ycoord), QPoint(dialData::fMajMrkXEnd, ycoord));
                painter->drawText(QRect(0, ycoord - 7, 45, 12), Qt::AlignRight, QString::number(markFreq));
                markFreq += 5;
            }
            else
            {
                painter->drawLine(QPoint(dialData::fMinMrkXStart, ycoord), QPoint(dialData::fMinMrkXEnd, ycoord));
            }

            markCount++;

        }
    }
    else if (dialData::minorMarker[zoomLevel] == 2)
    {
        for (int ycoord = 0; ycoord < maxScaleY; ycoord += markStep)
        {

            if (markCount == 0 || markCount % 10 == 0)
            {
                painter->drawLine(QPoint(dialData::fMajMrkXStart, ycoord), QPoint(dialData::fMajMrkXEnd, ycoord));
                painter->drawText(QRect(0, ycoord - 7, 45, 12), Qt::AlignRight, QString::number(markFreq));
                markFreq += 10;
            }
            else
            {
                painter->drawLine(QPoint(dialData::fMinMrkXStart, ycoord), QPoint(dialData::fMinMrkXEnd, ycoord));
            }


            markCount++;
        }


    }
    else if (dialData::minorMarker[zoomLevel] == 3)
    {
        for (int ycoord = 0; ycoord < maxScaleY; ycoord += markStep)
        {

            if (markCount == 0 || markCount % 2 == 0)
            {
                painter->drawLine(QPoint(dialData::fMajMrkXStart, ycoord), QPoint(dialData::fMajMrkXEnd, ycoord));
                painter->drawText(QRect(0, ycoord - 7, 45, 12), Qt::AlignRight, QString::number(markFreq));
                markFreq += 50;
            }
            else
            {
                painter->drawLine(QPoint(dialData::fMinMrkXStart, ycoord), QPoint(dialData::fMinMrkXEnd, ycoord));
            }

            markCount++;

        }

    }

}


void FreqDial::drawCursor(QPainter *painter, double frequency)
{


    qint32 fmaj = 0;
    qint32 fmin = 0;
    fmaj = frequency/1000;
    fmin = frequency - (fmaj*1000);
    qint32 offsetFreq = fmaj - scaleStartFreq;
    qint32 cursorY = (offsetFreq * dialData::khzPixelStep[zoomLevel]) + (fmin/dialData::hzPixelStep[zoomLevel]);


    QPolygon freqCursor;

    freqCursor << QPoint(60,cursorY-5);
    freqCursor << QPoint(60,cursorY+10);
    freqCursor << QPoint(70,cursorY);
    freqCursor << QPoint(60,cursorY-5);

    QBrush freqCursorBrush(Qt::red, Qt::SolidPattern);

    QPainterPath freqCursorPath;
    freqCursorPath.addPolygon(freqCursor);
    painter->fillPath(freqCursorPath, freqCursorBrush);



}



void FreqDial::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}
