/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <QStringList>
#include <QFont>

#include "minoscompass.h"
#include <QtDebug>

MinosCompass::MinosCompass(QWidget *parent)

    : QWidget(parent)

{

   compassDialBearing = 0;


}


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



    QString degLegends = " ,20,40,60,80, ,100,120,140,160, ,200,220,240,260, ,280,300,320,340";
    int degreeRotate[] = {20,20,20,20,10,10,20,20,20,20,20,20,20,20,10,10,20,20,20,20};
    QStringList legendsList = degLegends.split(",");
    QRect textRect1(-8, -90, 20,15);

    for (int j = 0; j < 20; j++)
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

