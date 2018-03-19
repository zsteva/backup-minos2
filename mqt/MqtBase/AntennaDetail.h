#ifndef ANTENNADETAIL_H
#define ANTENNADETAIL_H
#include "base_pch.h"

#include "PubSubValue.h"

const QString AntennaDetailType("AntennaDetail");
class AntennaDetail: public PubSubValue
{
    MinosItem<int> _minAzimuth;
    MinosItem<int> _maxAzimuth;

public:
    AntennaDetail(): PubSubValue(AntennaDetailType){}
    AntennaDetail(QString s);
    AntennaDetail(int minA, int maxA): PubSubValue(AntennaDetailType),  _minAzimuth(minA), _maxAzimuth(maxA){}
    ~AntennaDetail();

    bool isDirty();
    void clearDirty();

    virtual QString pack();
    virtual void unpack(QString);


    int minAzimuth() const;
    int maxAzimuth() const;
    void setMinAzimuth(int minAzimuth);
    void setMaxAzimuth(int maxAzimuth);
};

#endif // ANTENNADETAIL_H
