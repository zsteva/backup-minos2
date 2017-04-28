#ifndef CALLSIGNMARKER_H
#define CALLSIGNMARKER_H

#include <QDateTime>
#include <QString>
#include <QMetaType>

class CallsignMarker
{
public:
    CallsignMarker();
    CallsignMarker(QString callsign, QString logtime, int type, int bearing);



    void setcallSign(QString callsign);
    QString getcallSign();
    void setmarkerTime(QString logtime);
    QString getmarkerTime();
    void setspotType(int spot_type);
    int getspotType();
    void setcallBearing(int bearing);
    int getcallBearing();



    void addCallsignMarker(QString callsign, QString time, int spotType, int callBearing);
private:


        QString callSign;
        //QDateTime time;
        QString logTime;
        int spotType;
        int callBearing;
        //int markerPtr;



};


Q_DECLARE_METATYPE(CallsignMarker)


#endif // CALLSIGNMARKER_H
