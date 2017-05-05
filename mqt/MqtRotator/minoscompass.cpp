/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
//
/////////////////////////////////////////////////////////////////////////////





#include <QtWidgets>
#include <QStringList>
#include <QFont>

#include "minoscompass.h"
#include <QtDebug>

/*
// for test
#include <QTimer>
#include <QString>
// for test
*/

MinosCompass::MinosCompass(QWidget *parent)

    : QDial(parent)

{

   compassDialBearing = 0;
   setSizePolicy(QSizePolicy:: Preferred, QSizePolicy:: Preferred);
/*
   // for test...
   QTimer *timer = new QTimer(this);
   connect(this, SIGNAL(bearing_updated(QString)), this, SLOT(compassDialUpdate(const QString &)));
   connect(timer, SIGNAL(timeout()), this, SLOT(upDateDial()));
    timer->start();
   // for test
*/
}


/*
// for test
void MinosCompass::upDateDial()
{

    static int bearing = 0;

    bearing += 1;
    if (bearing > 359)
    {
        bearing = 0;
    }
    QString s = QString::number(bearing);
    emit bearing_updated(s);
}
// for test
*/
void MinosCompass::paintEvent(QPaintEvent *)
{


    static const QPoint needleFront[3] = {
        QPoint(7, 0),
        QPoint(-7, 0),
        QPoint(0, -80)
    };

    static const QPoint needleBack[3] = {
        QPoint(7, 0),
        QPoint(-7, 0),
        QPoint(0, 80)
    };


    QColor tenDegreeMarkerColor(127, 0, 127);

    QColor twoDegreeMarkerColor(0, 127, 127, 191);


    QColor needleFrontColor("red");

    QColor needleBackColor("gray");



    int side = qMin(width(), height());

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(width() / 2, height() / 2);

    painter.scale(side / 200.0, side / 200.0);



    painter.setPen(Qt::NoPen);


    // paint ten degree markers
    painter.setPen(tenDegreeMarkerColor);


    for (int i = 0; i < 36; ++i) {
       painter.drawLine(88, 0, 96, 0);
       painter.rotate(10.0);
    }

    painter.setPen(Qt::NoPen);


    painter.setBrush(needleFrontColor);

    painter.save();

    painter.rotate(compassDialBearing);
    painter.drawConvexPolygon(needleFront, 3);

    painter.setBrush(needleBackColor);
    painter.drawConvexPolygon(needleBack, 3);

    painter.restore();


    painter.setPen(twoDegreeMarkerColor);


    for (int j = 0; j < 180; ++j) {
        if ((j % 5) != 0){
            painter.drawLine(92, 0, 96, 0);
       }
       painter.rotate(2);
    }

     painter.setPen("black");



    QString degLegends = " ,30,60, ,120,150, ,210,240, ,300,330";
    int degreeRotate[] = {30,30,30,30,30,30,30,30,30,30,30,30};
    QStringList legendsList = degLegends.split(",");
    QRect textRect1(-8, -90, 20,15);

    for (int j = 0; j < 12; j++)
    {
        painter.drawText(textRect1,legendsList.value(j));
        painter.rotate(degreeRotate[j]);
    }

    QString dirLegends = "N,E,S,W";
    QStringList dirLegendsList = dirLegends.split(",");

    QRect textRect2(-3, -90, 15,15);
//    QFont textFont();
//    textFont.setWeight( QFont::Bold );
//   painter.setfont(textFont);
    for (int j = 0; j < 4; j++)
    {
        painter.drawText(textRect2,dirLegendsList.value(j));
        painter.rotate(90);
    }

    painter.setPen( Qt::red );
    painter.drawEllipse( QRect( -10, 30, 15, 15 ) );


}


void MinosCompass::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        qDebug() << "mouse position" << lastPoint;
    }
}






void MinosCompass::compassDialUpdate(const QString bearing)
{
    bool ok;
    qDebug() << "bearing to compass " << bearing;
    compassDialBearing = bearing.toInt(&ok, 10);
    qDebug() << "int bearing to compass " << compassDialBearing;
    if (ok)
    {
        update();
    }
}

