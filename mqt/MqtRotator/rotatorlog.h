/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
//
/////////////////////////////////////////////////////////////////////////////




#ifndef ROTATORLOG_H
#define ROTATORLOG_H

#include <QObject>

class RotatorLog
{
public:
    RotatorLog();
    int writeLog(const QString);

private:
    QString oldBearing;
    bool firstBearing;
    bool moving;
    int writeBearing(const QString bearing);

};

#endif // ROTATORLOG_H
