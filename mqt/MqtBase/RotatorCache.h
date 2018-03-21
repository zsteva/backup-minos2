#ifndef ROTATORCACHE_H
#define ROTATORCACHE_H

#include "base_pch.h"
#include "AntennaDetail.h"
#include "AntennaState.h"

class RotatorCache
{
    QMap<QString, AntennaDetail> rotDetails;
    QMap<QString, AntennaState> rotStates;
public:
    RotatorCache();

    QString getDetailString(const QString &name) const;
    void setDetailString(const AnalysePubSubNotify &an);

    QString getStateString(const QString &name) const;
    void setStateString(const AnalysePubSubNotify & an);

    void setDetail(const QString &name, const AntennaDetail &detail);
    void setState(const QString &name, const AntennaState &state);

    void setSelected(const QString &name, const QString &sel);
    void setStatus(const QString &name, const QString &state);
    void setBearing(const QString &name, int bearing);

    void setMinAzimuth(const QString &name, int minaz);
    void setMaxAzimuth(const QString &name, int maxaz);

    void publishState( );
    void publishDetails( );
};

#endif // ROTATORCACHE_H
