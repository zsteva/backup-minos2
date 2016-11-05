#include "rotatorlog.h"
#include <QFile>
#include <QtDebug>
#include <QDateTime>

RotatorLog::RotatorLog()
{
    oldBearing = "";
    firstBearing = false;
    moving = false;
}



int RotatorLog::writeLog(const QString bearing)
{

    if (oldBearing != bearing && !firstBearing)
    {
        oldBearing = bearing;
        writeBearing(bearing);
        firstBearing = true;
        return 0;
    }
    if (oldBearing != bearing && firstBearing)
    {
        oldBearing = bearing;
        moving = true;
        return 0;
    }
    if (moving)
    {
        if (oldBearing == bearing && firstBearing)
        {
            moving = false;
            writeBearing(bearing);
        }
    }
    return 0;
}



int RotatorLog::writeBearing(const QString bearing)
{
        QDateTime t = QDateTime::currentDateTime();
        QString t_str = t.toString();

        QFile file( "testfile.txt" );
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



