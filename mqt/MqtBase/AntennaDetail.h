#ifndef ANTENNADETAIL_H
#define ANTENNADETAIL_H
#include "base_pch.h"

const QString AntennaDetailType("AntennaDetail");


class AntennaDetail: public PubSubValue
{
    MinosItem<int> _minAzimuth;
    MinosItem<int> _maxAzimuth;
public:
    AntennaDetail();
    AntennaDetail(QString s);
    AntennaDetail(int minA, int maxA): PubSubValue(AntennaDetailType),  _minAzimuth(minA), _maxAzimuth(maxA){}

    bool isDirty() const;
    void clearDirty();
    void setDirty();

    virtual QString pack() const;
    virtual void unpack(QString);

    MinosItem<int> minAzimuth() const;
    MinosItem<int> maxAzimuth() const;
    void setMinAzimuth(int minAzimuth);
    void setMaxAzimuth(int maxAzimuth);

};

#endif // ANTENNADETAIL_H
