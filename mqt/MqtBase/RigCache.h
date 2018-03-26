#ifndef RIGCACHE_H
#define RIGCACHE_H

#include "base_pch.h"
#include "RigState.h"

class RigCache
{
    QMap<PubSubName, RigState> rigStates;
    QVector<PubSubName> rigList;
public:
    RigCache();

    void publishState();

    QString getStateString(const PubSubName &name) const;
    void setStateString(const AnalysePubSubNotify & an);

    void setState(const PubSubName &name, RigState &state);

    PubSubName getSelected();

    void setSelected(const PubSubName &name, const QString &selected);
    void setStatus(const PubSubName &name, const QString &status);
    void setFreq(const PubSubName &name, double freq);
    void setMode(const PubSubName &name, const QString &mode);
    void setTransverterOffset(const PubSubName &name, double transverterOffset);
    void setTransverterSwitch(const PubSubName &name, int transverterSwitch);
    void setTransverterStatus(const PubSubName &name, bool transverterStatus);

    void addRigList(const QString &s);
    QVector<PubSubName> getRigList()
    {
        return rigList;
    }
};

#endif // RIGCACHE_H
