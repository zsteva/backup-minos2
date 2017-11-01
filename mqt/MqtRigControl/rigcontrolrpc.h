/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rig Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2017
//
// Interprocess Control Logic
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2017
//
//
//
/////////////////////////////////////////////////////////////////////////////






#ifndef RIGCONTROLRPC_H
#define RIGCONTROLRPC_H

#include "base_pch.h"

class RigControlMainWindow;

class RigControlRpc : public QObject
{
    Q_OBJECT

    RigControlMainWindow *parent;


public:
    explicit RigControlRpc(RigControlMainWindow *parent);

    void publishState(const QString &state);
    void publishRadioName(const QString &radioName);
    void publishRxPbFlag(const QString &flag);
    void publishFreq(const QString &freq);
    void publishMode(const QString &mode);
    void publishPbandState(const QString &state);
signals:
    void setFreq(QString);
    void setMode(QString);
    void setPassBand(int);
public slots:


private slots:
    void on_notify(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from );
    void on_serverCall( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );

};

#endif // RIGCONTROLRPC_H
