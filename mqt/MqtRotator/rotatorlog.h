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
    int writeLog(int);

public slots:
    void readBearingLogConfig();
private:
    QString oldBearing;
    QString bearingLogDir;
    QString bearingLogFileName;
    int bearingLogBearingDiff;
    bool bearingLogEnabled;
    bool firstBearing;
    bool moving;
    int writeBearing(const QString, QString);

};

#endif // ROTATORLOG_H
