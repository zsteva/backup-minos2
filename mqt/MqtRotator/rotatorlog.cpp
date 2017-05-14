/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
//
/////////////////////////////////////////////////////////////////////////////




#include "rotatorlog.h"
#include <QFile>
#include <QtDebug>
#include <QDateTime>
#include <QSettings>

RotatorLog::RotatorLog()
{
    oldBearing = "";
    firstBearing = false;
    moving = false;
}



int RotatorLog::writeLog(int bearing)
{
    QString fileName = bearingLogDir;
    if (bearingLogEnabled)
    {

       fileName.append('/');
       fileName.append(bearingLogFileName);

       QString sbearing = sbearing.setNum(bearing);
        if (oldBearing != sbearing && !firstBearing)
        {
            oldBearing = sbearing;
            writeBearing(sbearing, fileName);
            firstBearing = true;
            return 0;
        }
        if (oldBearing != sbearing && firstBearing)
        {
            oldBearing = sbearing;
            moving = true;
            return 0;
        }
        if (moving)
        {
            if (oldBearing == sbearing && firstBearing)
            {
                moving = false;
                writeBearing(sbearing, fileName);
            }
        }
     }

    return 0;
}



int RotatorLog::writeBearing(const QString bearing, QString filename)
{
        QDateTime t = QDateTime::currentDateTime();
        QString t_str = t.toString();

        QFile file( filename );
        if( !file.exists() )
        {
            qDebug() << "The file" << file.fileName() << "does not exist.";
            //return -1;
        }
        if( !file.open( QIODevice::Append | QIODevice::Text) )
        {
            qDebug() << "Could not open" << file.fileName() << "for writing.";
            //return -1;
        }
        qDebug() << "The file opened.";

        QTextStream stream(&file);

        stream << t_str << " Bearing is " + bearing + " Degrees" + "\n";

        file.close();
        return 0;
}


void RotatorLog::readBearingLogConfig()
{
    QSettings config("./Configuration/MinosRotatorConfig.ini", QSettings::IniFormat);
    config.beginGroup("BearingLog");

    bearingLogDir = config.value("directory", "./Configuration/BearingLog").toString();
    bearingLogFileName = config.value("filename", "bearingLog").toString();
    bearingLogBearingDiff = config.value("bearing_difference", 0).toInt();
    bearingLogEnabled = config.value("loggingEnabled", false).toBool();

    config.endGroup();
}




