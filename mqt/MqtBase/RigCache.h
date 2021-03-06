#ifndef RIGCACHE_H
#define RIGCACHE_H

#include "base_pch.h"
#include "RigState.h"
#include "RigDetails.h"

class RigCache
{
    QMap<PubSubName, RigState> rigStates;
    QMap<PubSubName, RigDetails> rigDetails;
    QVector<PubSubName> rigList;
    void publishState();
    void publishDetails();
public:
    RigCache();

    void invalidate();
    void invalidate(const PubSubName &name);
    void publish()
    {
        publishState();
        publishDetails();
    }

    QString getStateString(const PubSubName &name) const;
    void setStateString(const AnalysePubSubNotify & an);
    QString getDetailsString(const PubSubName &name) const;
    void setDetailsString(const AnalysePubSubNotify & an);

    void setState(const PubSubName &name, const RigState &state);
    void setDetails(const PubSubName &name, const RigDetails &details);

    PubSubName getSelected();

    RigState &getState(const PubSubName &p);
    RigDetails &getDetails(const PubSubName &p);
    void setSelected(const PubSubName &name, const QString &selected);
    void setStatus(const PubSubName &name, const QString &status);
    void setFreq(const PubSubName &name, double freq);
    void setMode(const PubSubName &name, const QString &mode);
    void setTransverterOffset(const PubSubName &name, double transverterOffset);
    void setTransverterSwitch(const PubSubName &name, int transverterSwitch);
    void setTransverterStatus(const PubSubName &name, bool transverterStatus);
    void setBandList(const PubSubName &name, const QString &bands);

    void addRigList(const QString &s);
    QVector<PubSubName> getRigList()
    {
        return rigList;
    }
};

#endif // RIGCACHE_H
