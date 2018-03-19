#include "RigCache.h"

RigCache::RigCache()
{

}
QString RigCache::getStateString(const QString &name)
{
    QString val = rigStates[name].pack();
    return val;
}
void RigCache::setStateString(const QString &name, const QString &value)
{
    RigState &as = rigStates[name];
    as.unpack(value);
}

void RigCache::setState(QString name, RigState &state)
{
    rigStates[name] = state;
}

void RigCache::setSelected(const QString &name, const QString &sel)
{
    for(QMap<QString, RigState>::iterator i = rigStates.begin(); i != rigStates.end(); i++ )
    {
        if (i.key() == name)
            i.value().setSelected(sel);
        else
            i.value().setSelected("");
    }
}
void RigCache::setStatus(const QString &name, const QString &status)
{
    rigStates[name].setStatus(status);
}

void RigCache::setFreq(const QString &name, double freq)
{
    rigStates[name].setFreq(freq);
}
void RigCache::setMode(const QString &name, const QString &mode)
{
    rigStates[name].setMode(mode);
}
void RigCache::setTransverterOffset(const QString &name, double transverterOffset)
{
    rigStates[name].setTransverterOffset(transverterOffset);
}
void RigCache::setTransverterSwitch(const QString &name, int transverterSwitch)
{
    rigStates[name].setTransverterSwitch(transverterSwitch);
}
void RigCache::setTransverterStatus(const QString &name, bool transverterStatus)
{
    rigStates[name].setTransverterStatus(transverterStatus);
}
void RigCache::publishState()
{
    MinosRPC *rpc = MinosRPC::getMinosRPC();
    for(QMap<QString, RigState>::iterator i = rigStates.begin(); i != rigStates.end(); i++ )
    {
        if (i.value().isDirty())
        {
            rpc->publish(rpcConstants::rigStateCategory, i.key(), i.value().pack(), psPublished);
            rigStates[i.key()].clearDirty();
        }
    }

}
