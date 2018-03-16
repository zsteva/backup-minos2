#ifndef RIGSTATE_H
#define RIGSTATE_H
#include "PubSubValue.h"

const QString RigStateType("RigState");
class RigState: public PubSubValue
{
    QString _selected;
    double _freq;
    QString _mode;
    double _transverterOffset;
    int _transverterSwitch;
    QString _transverterStatus;

public:
    RigState(QString s);
    RigState(QString sel, int f, int m, double tvo, int tvsw, QString tvst)
        :PubSubValue(RigStateType), _selected(sel), _freq(f), _mode(m), _transverterOffset(tvo), _transverterSwitch(tvsw), _transverterStatus(tvst){}
    ~RigState();

    virtual QString pack();
    virtual void unpack(QString);

    QString selected() const;
    double freq() const;
    QString mode() const;
    double transverterOffset() const;
    int transverterSwitch() const;
    QString transverterStatus() const;
};

#endif // RIGSTATE_H
