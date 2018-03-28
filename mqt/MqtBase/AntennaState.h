#ifndef ANTENNASTATE_H
#define ANTENNASTATE_H
#include "base_pch.h"
#include "PubSubValue.h"

const QString AntennaStateType("AntennaState");

class AntennaState: public PubSubValue
{
    MinosStringItem<QString> _bearing;
    MinosStringItem<QString> _state;
    MinosStringItem<QString> _selected;
public:
    AntennaState():PubSubValue(AntennaStateType){}
    AntennaState(QString);
    AntennaState(const QString &st, const QString &sel, const QString &b):PubSubValue(AntennaStateType)
    {
        _bearing.setValue(b);
        _state.setValue(st);
        _selected.setValue(sel);
    }
    ~AntennaState();

    virtual QString pack() const;
    virtual void unpack(QString);
    bool isDirty() const;
    void clearDirty();
    void setDirty();

    QString bearing() const;
    QString selected() const;
    QString state() const;
    void setBearing(const QString &bearing);
    void setState(const QString &state);
    void setSelected(const QString &selected);
};

#endif // ANTENNASTATE_H
