/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
//
/////////////////////////////////////////////////////////////////////////////




#ifndef ROTATORRPC_H
#define ROTATORRPC_H

#include "base_pch.h"
#include "RotatorCache.h"

class RotatorMainWindow;

class RotatorRpc : public QObject
{
    Q_OBJECT

    RotatorMainWindow *parent;
public:
    explicit RotatorRpc(RotatorMainWindow *parent);

    RotatorCache rotatorCache;

    void publishAntennaList(QString ants);
    void publishState( const QString &name, const QString &state );
    void publishBearing( const QString &name, const QString bearing );
    void publishMaxAzimuth(const QString &name, const QString maxAzimuth);
    void publishMinAzimuth(const QString &name, const QString minAzimuth);

    void flagStatus(QString msg);

signals:
    void setRotation(int direction, int angle);
    void selectAntenna(QString);

private slots:
    void on_notify(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from );
    void on_serverCall( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );
};

#endif // ROTATORRPC_H
