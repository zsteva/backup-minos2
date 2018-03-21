#ifndef RIGCACHE_H
#define RIGCACHE_H

#include "base_pch.h"
#include "RigState.h"

class RigCache
{
    QMap<QString, RigState> rigStates;
public:
    RigCache();

    void publishState();

    QString getStateString(const QString &name) const;
    void setStateString(const AnalysePubSubNotify & an);

    void setState(QString name, RigState &state);

    void setSelected(const QString &name, const QString &selected);
    void setStatus(const QString &name, const QString &status);
    void setFreq(const QString &name, double freq);
    void setMode(const QString &name, const QString &mode);
    void setTransverterOffset(const QString &name, double transverterOffset);
    void setTransverterSwitch(const QString &name, int transverterSwitch);
    void setTransverterStatus(const QString &name, bool transverterStatus);
};

#endif // RIGCACHE_H
