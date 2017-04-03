/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
//
/////////////////////////////////////////////////////////////////////////////




#ifndef MINOSCOMPASS_H
#define MINOSCOMPASS_H

#include <QWidget>


class MinosCompass : public QWidget
{
    Q_OBJECT

public:
    MinosCompass(QWidget *parent = 0);

public slots:

    void compassDialUpdate(const QString);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;


    void mousePressEvent(QMouseEvent *event);

private:
    int compassDialBearing;
    QPoint lastPoint;
};


#endif
