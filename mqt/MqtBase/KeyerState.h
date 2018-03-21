#ifndef KEYERSTATE_H
#define KEYERSTATE_H
#include "PubSubValue.h"

const QString KeyerStateType("KeyerState");

class KeyerState: public PubSubValue
{
    QString _report;
    QString _selected;
public:
    KeyerState(QString s);
    KeyerState(QString st, QString sel):PubSubValue(KeyerStateType), _report(st), _selected(sel){}
    ~KeyerState();

    virtual QString pack() const;
    virtual void unpack(QString);
    QString report() const;
    QString selected() const;
};

#endif // KEYERSTATE_H
