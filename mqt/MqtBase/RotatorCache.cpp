#include "RotatorCache.h"
//#include "RPCCommandConstants.h"
#include "base_pch.h"

RotatorCache::RotatorCache()
{

}

QString RotatorCache::getDetailString(const QString &name) const
{
    QString val = rotDetails[name].pack();
    return val;
}
void RotatorCache::setDetailString(const AnalysePubSubNotify & an)
{
    AntennaDetail &ad = rotDetails[an.getKey()];
    ad.unpack(an.getValue());
}
QString RotatorCache::getStateString(const QString &name) const
{
    QString val = rotStates[name].pack();
    return val;
}
void RotatorCache::setStateString(const AnalysePubSubNotify &an)
{
    AntennaState &as = rotStates[an.getKey()];
    as.unpack(an.getValue());
}

//QMap<QString, AntennaDetail> rotDetails;
//QMap<QString, AntennaState> rotStates;

void RotatorCache::setDetail(const QString &name, const AntennaDetail &detail)
{
    rotDetails[name] = detail;
}
void RotatorCache::setState(const QString &name, const AntennaState &state)
{
    rotStates[name] = state;
}

void RotatorCache::setSelected(const QString &name, const QString &sel)
{
    for(QMap<QString, AntennaState>::iterator i = rotStates.begin(); i != rotStates.end(); i++ )
    {
        if (i.key() == name)
            i.value().setSelected(sel);
        else
            i.value().setSelected("");
    }
}
void RotatorCache::setStatus(const QString &name, const QString &state)
{
    rotStates[name].setState(state);
}
void RotatorCache::setBearing(const QString &name, int bearing)
{
    rotStates[name].setBearing(bearing);
}
void RotatorCache::setMinAzimuth(const QString &name, int minaz)
{
    rotDetails[name].setMinAzimuth(minaz);
}
void RotatorCache::setMaxAzimuth(const QString &name, int maxaz)
{
    rotDetails[name].setMaxAzimuth(maxaz);
}

void RotatorCache::publishState()
{
    MinosRPC *rpc = MinosRPC::getMinosRPC();
    for(QMap<QString, AntennaState>::iterator i = rotStates.begin(); i != rotStates.end(); i++ )
    {
        if (i.value().isDirty())
        {
            rpc->publish(rpcConstants::rotatorDetailCategory, i.key(), i.value().pack(), psPublished);
            rotStates[i.key()].clearDirty();
        }
    }

}
void RotatorCache::publishDetails()
{
    MinosRPC *rpc = MinosRPC::getMinosRPC();
    for(QMap<QString, AntennaDetail>::iterator i = rotDetails.begin(); i != rotDetails.end(); i++ )
    {
        if (i.value().isDirty())
        {
            rpc->publish(rpcConstants::rotatorStateCategory, i.key(), i.value().pack(), psPublished);
            rotDetails[i.key()].clearDirty();
        }
    }

}
