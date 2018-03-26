#include "RigCache.h"

RigCache::RigCache()
{

}
QString RigCache::getStateString(const PubSubName &name) const
{
    QString val = rigStates[name].pack();
    return val;
}
void RigCache::setStateString(const AnalysePubSubNotify & an)
{
    RigState &as = rigStates[PubSubName(an)];
    as.unpack(an.getValue());
}
void RigCache::addRigList(const QString &s)
{
    QStringList list = s.split(":");
    foreach(QString l, list)
    {
        PubSubName psn(l);
        if (!rigList.contains(psn))
            rigList.push_back(psn);
    }
    qSort(rigList);
}

void RigCache::setState(const PubSubName &name, RigState &state)
{
    rigStates[name] = state;
}

void RigCache::setSelected(const PubSubName &name, const QString &sel)
{
    for(QMap<PubSubName, RigState>::iterator i = rigStates.begin(); i != rigStates.end(); i++ )
    {
        if (i.key() == name)
            i.value().setSelected(sel);
        else
            i.value().setSelected("");
    }
}
PubSubName RigCache::getSelected()
{
    for(QMap<PubSubName, RigState>::iterator i = rigStates.begin(); i != rigStates.end(); i++ )
    {
        if (!i.value().selected().isEmpty())
        {
            PubSubName psn = i.key();
            return psn;
        }
    }
    return PubSubName();
}
void RigCache::setStatus(const PubSubName &name, const QString &status)
{
    rigStates[name].setStatus(status);
}

void RigCache::setFreq(const PubSubName &name, double freq)
{
    rigStates[name].setFreq(freq);
}
void RigCache::setMode(const PubSubName &name, const QString &mode)
{
    rigStates[name].setMode(mode);
}
void RigCache::setTransverterOffset(const PubSubName &name, double transverterOffset)
{
    rigStates[name].setTransverterOffset(transverterOffset);
}
void RigCache::setTransverterSwitch(const PubSubName &name, int transverterSwitch)
{
    rigStates[name].setTransverterSwitch(transverterSwitch);
}
void RigCache::setTransverterStatus(const PubSubName &name, bool transverterStatus)
{
    rigStates[name].setTransverterStatus(transverterStatus);
}
void RigCache::publishState()
{
    MinosRPC *rpc = MinosRPC::getMinosRPC();
    for(QMap<PubSubName, RigState>::iterator i = rigStates.begin(); i != rigStates.end(); i++ )
    {
        if (i.value().isDirty())
        {
            rpc->publish(rpcConstants::rigStateCategory, i.key().key(), i.value().pack(), psPublished);
            rigStates[i.key()].clearDirty();
        }
    }

}
