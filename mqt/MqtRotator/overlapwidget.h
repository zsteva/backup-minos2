/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) D. G. Balharrie M0DGB/G8FKH
//
//
/////////////////////////////////////////////////////////////////////////////




#ifndef OVERLAPWIDGET_H
#define OVERLAPWIDGET_H

#include <QWidget>

class OverLapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OverLapWidget(QWidget *parent = 0);
    void setText(QString text);
signals:

public slots:
    void overlapDisplayUpdate(bool);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;


private:
    bool overlapOn;

};

#endif // OVERLAPWIDGET_H
