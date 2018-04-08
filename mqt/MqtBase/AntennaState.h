#ifndef ANTENNASTATE_H
#define ANTENNASTATE_H
#include "base_pch.h"

const QString AntennaStateType("AntennaState");

class AntennaState: public PubSubValue
{
    MinosStringItem<QString> _bearing;
    MinosStringItem<QString> _state;
    MinosStringItem<QString> _selected;
public:
    AntennaState();
    AntennaState(QString);
    AntennaState(const QString &st, const QString &sel, const QString &b);

    virtual QString pack() const;
    virtual void unpack(QString);
    bool isDirty() const;
    void clearDirty();
    void setDirty();

    MinosStringItem<QString> bearing() const;
    MinosStringItem<QString> selected() const;
    MinosStringItem<QString> state() const;
    void setBearing(const QString &bearing);
    void setState(const QString &state);
    void setSelected(const QString &selected);
};

#endif // ANTENNASTATE_H
