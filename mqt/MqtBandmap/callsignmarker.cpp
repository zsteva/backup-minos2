#include "callsignmarker.h"

CallsignMarker::CallsignMarker()
{

}

CallsignMarker::CallsignMarker(QString callsign, QString logtime, int type, int bearing)
{
    callSign = callsign;
    logTime = logtime;
    spotType = type;
    callBearing = bearing;
}


void CallsignMarker::setcallSign(QString callsign)
{
    callSign = callsign;
}

QString CallsignMarker::getcallSign()
{
    return callSign;
}

void CallsignMarker::setmarkerTime(QString logtime)
{
    logTime = logtime;
}

QString CallsignMarker::getmarkerTime()
{
    return logTime;
}

void CallsignMarker::setspotType(int spot_type)
{
    spotType = spot_type;
}

int CallsignMarker::getspotType()
{
    return spotType;
}


void CallsignMarker::setcallBearing(int bearing)
{
    callBearing = bearing;
}

int CallsignMarker::getcallBearing()
{
    return callBearing;
}


