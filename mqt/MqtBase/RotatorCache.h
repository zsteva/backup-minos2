#ifndef ROTATORCACHE_H
#define ROTATORCACHE_H

#include "base_pch.h"
#include "AntennaDetail.h"
#include "AntennaState.h"

class RotatorCache
{
    QMap<PubSubName, AntennaDetail> rotDetails;
    QMap<PubSubName, AntennaState> rotStates;
    QVector<PubSubName> rotList;
public:
    RotatorCache();

    void invalidate();

    AntennaState &getState(const PubSubName &p);
    AntennaDetail &getDetails(const PubSubName &p);

    QString getDetailString(const PubSubName &name) const;
    void setDetailString(const AnalysePubSubNotify &an);

    QString getStateString(const PubSubName &name) const;
    void setStateString(const AnalysePubSubNotify & an);

    void setDetail(const PubSubName &name, const AntennaDetail &detail);
    void setState(const PubSubName &name, const AntennaState &state);

    PubSubName getSelected();

    void setSelected(const PubSubName &name, const QString &sel);
    void setStatus(const PubSubName &name, const QString &state);
    void setBearing(const PubSubName &name, const QString &bearing);

    void setMinAzimuth(const PubSubName &name, int minaz);
    void setMaxAzimuth(const PubSubName &name, int maxaz);

    void setRotatorPresets(const PubSubName &name, const QString &p);

    void publishState( );
    void publishDetails( );

    void addRotList(const QString &s);
    QVector<PubSubName> getRotList()
    {
        return rotList;
    }

};

#endif // ROTATORCACHE_H
