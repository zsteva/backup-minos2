#ifndef RIGDETAILS_H
#define RIGDETAILS_H
#include "base_pch.h"
#include "PubSubValue.h"

const QString RigDetailsType("RigDetails");
class RigDetails: public PubSubValue
{
    MinosItem<double> _transverterOffset;
    MinosItem<int> _transverterSwitch;
    MinosItem<bool> _transverterStatus;
    MinosStringItem<QString> _bandList;

public:
    RigDetails();
    RigDetails(QString s);
    RigDetails(double tvo, int tvsw, bool tvst, QString blist);

    bool isDirty() const;
    void clearDirty();
    void setDirty();

    virtual QString pack() const;
    virtual void unpack(QString);

    double transverterOffset() const;
    int transverterSwitch() const;
    bool transverterStatus() const;

    void setTransverterOffset(double transverterOffset);
    void setTransverterSwitch(int transverterSwitch);
    void setTransverterStatus(bool transverterStatus);
    QString bandList() const;
    void setBandList(const QString &bandList);
};


#endif // RIGDETAILS_H
