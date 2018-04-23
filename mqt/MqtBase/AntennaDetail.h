#ifndef ANTENNADETAIL_H
#define ANTENNADETAIL_H
#include "base_pch.h"

const QString AntennaDetailType("AntennaDetail");


class AntennaDetail: public PubSubValue
{
    MinosItem<int> _minAzimuth;
    MinosItem<int> _maxAzimuth;
    MinosStringItem<QString> _selected;
public:
    AntennaDetail();
    AntennaDetail(QString s);
    AntennaDetail(int minA, int maxA, const QString &sel):
        PubSubValue(AntennaDetailType)
    {
          _minAzimuth.setValue(minA);
          _maxAzimuth.setValue(maxA);
          _selected.setValue(sel);
    }

    bool isDirty() const;
    void clearDirty();
    void setDirty();

    virtual QString pack() const;
    virtual void unpack(QString);

    MinosItem<int> minAzimuth() const;
    MinosItem<int> maxAzimuth() const;
    MinosStringItem<QString> selected() const;
    void setMinAzimuth(int minAzimuth);
    void setMaxAzimuth(int maxAzimuth);
    void setSelected(const QString &selected);
};

#endif // ANTENNADETAIL_H
