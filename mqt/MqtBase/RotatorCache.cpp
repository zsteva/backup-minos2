#include "RotatorCache.h"
//#include "RPCCommandConstants.h"
#include "base_pch.h"

RotatorCache::RotatorCache()
{

}
void RotatorCache::addRotList(const QString &s)
{
    QStringList list = s.split(":");
    foreach(QString l, list)
    {
        PubSubName psn(l);
        if (!rotList.contains(psn))
            rotList.push_back(psn);
    }
    qSort(rotList);
}

QString RotatorCache::getDetailString(const PubSubName &name) const
{
    QString val = rotDetails[name].pack();
    return val;
}
void RotatorCache::setDetailString(const AnalysePubSubNotify & an)
{
    AntennaDetail &ad = rotDetails[PubSubName(an)];
    ad.unpack(an.getValue());
}
QString RotatorCache::getStateString(const PubSubName &name) const
{
    QString val = rotStates[name].pack();
    return val;
}
void RotatorCache::setStateString(const AnalysePubSubNotify &an)
{
    AntennaState &as = rotStates[PubSubName(an)];
    as.unpack(an.getValue());
}

//QMap<QString, AntennaDetail> rotDetails;
//QMap<QString, AntennaState> rotStates;

void RotatorCache::setDetail(const PubSubName &name, const AntennaDetail &detail)
{
    rotDetails[name] = detail;
}
void RotatorCache::setState(const PubSubName &name, const AntennaState &state)
{
    rotStates[name] = state;
}

void RotatorCache::setSelected(const PubSubName &name, const QString &sel)
{
    for(QMap<PubSubName, AntennaState>::iterator i = rotStates.begin(); i != rotStates.end(); i++ )
    {
        if (i.key() == name)
            i.value().setSelected(sel);
        else
            i.value().setSelected("");
    }
}
PubSubName RotatorCache::getSelected()
{

    for(QMap<PubSubName, AntennaState>::iterator i = rotStates.begin(); i != rotStates.end(); i++ )
    {
        if (!i.value().selected().isEmpty())
        {
            PubSubName psn = i.key();
            return psn;
        }
    }
    return PubSubName();
}
void RotatorCache::setStatus(const PubSubName &name, const QString &state)
{
    rotStates[name].setState(state);
}
void RotatorCache::setBearing(const PubSubName &name, int bearing)
{
    rotStates[name].setBearing(bearing);
}
void RotatorCache::setMinAzimuth(const PubSubName &name, int minaz)
{
    rotDetails[name].setMinAzimuth(minaz);
}
void RotatorCache::setMaxAzimuth(const PubSubName &name, int maxaz)
{
    rotDetails[name].setMaxAzimuth(maxaz);
}

void RotatorCache::publishState()
{
    MinosRPC *rpc = MinosRPC::getMinosRPC();
    for(QMap<PubSubName, AntennaState>::iterator i = rotStates.begin(); i != rotStates.end(); i++ )
    {
        if (i.value().isDirty())
        {
            rpc->publish(rpcConstants::rotatorDetailCategory, i.key().key(), i.value().pack(), psPublished);
            rotStates[i.key()].clearDirty();
        }
    }

}
void RotatorCache::publishDetails()
{
    MinosRPC *rpc = MinosRPC::getMinosRPC();
    for(QMap<PubSubName, AntennaDetail>::iterator i = rotDetails.begin(); i != rotDetails.end(); i++ )
    {
        if (i.value().isDirty())
        {
            rpc->publish(rpcConstants::rotatorStateCategory, i.key().key(), i.value().pack(), psPublished);
            rotDetails[i.key()].clearDirty();
        }
    }

}
