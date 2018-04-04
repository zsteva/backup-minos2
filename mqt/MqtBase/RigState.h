#ifndef RIGSTATE_H
#define RIGSTATE_H
#include "base_pch.h"
#include "PubSubValue.h"

const QString RigStateType("RigState");
class RigState: public PubSubValue
{
    MinosStringItem<QString> _selected;
    MinosStringItem<QString> _status;
    MinosItem<double> _freq;
    MinosStringItem<QString> _mode;

public:
    RigState();
    RigState(QString s);
    RigState(const QString &status, const QString &sel, int f, const QString &m);


    bool isDirty() const;
    void clearDirty();
    void setDirty();

    virtual QString pack() const;
    virtual void unpack(QString);

    MinosStringItem<QString> selected() const;
    MinosStringItem<QString> status() const;
    MinosItem<double> freq() const;
    MinosStringItem<QString> mode() const;

    void setSelected(const QString &selected);
    void setFreq(double freq);
    void setMode(const QString &mode);
    void setStatus(const QString &status);
};

#endif // RIGSTATE_H
