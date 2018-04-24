/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2018
//
/////////////////////////////////////////////////////////////////////////////



#ifndef RIGDETAILS_H
#define RIGDETAILS_H
#include "base_pch.h"

const QString RigDetailsType("RigDetails");
class RigDetails: public PubSubValue
{
    MinosStringItem<QString> _selected;
    MinosItem<double> _transverterOffset;
    MinosItem<int> _transverterSwitch;
    MinosItem<bool> _transverterStatus;
    MinosStringItem<QString> _bandList;

public:
    RigDetails();
    RigDetails(QString s);
    RigDetails(double tvo, int tvsw, bool tvst, const QString &blist, const QString &sel);

    bool isDirty() const;
    void clearDirty();
    void setDirty();

    virtual QString pack() const;
    virtual void unpack(QString);

    MinosStringItem<QString> selected() const;
    MinosItem<double> transverterOffset() const;
    MinosItem<int> transverterSwitch() const;
    MinosItem<bool> transverterStatus() const;
    MinosStringItem<QString> bandList() const;

    void setSelected(const QString &selected);
    void setTransverterOffset(double transverterOffset);
    void setTransverterSwitch(int transverterSwitch);
    void setTransverterStatus(bool transverterStatus);
    void setBandList(const QString &bandList);
};


#endif // RIGDETAILS_H
