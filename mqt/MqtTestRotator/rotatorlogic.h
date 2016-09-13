#ifndef ROTATORLOGIC_H
#define ROTATORLOGIC_H

#include "base_pch.h"

class RotatorMainWindow;

class RotatorLogic : public QObject
{
    Q_OBJECT

    RotatorMainWindow *parent;

public:
    explicit RotatorLogic(RotatorMainWindow *parent);

    void publishState( const QString &state );

signals:
    void setRotation(int direction, int angle);

private slots:
    void on_notify(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from );
    void on_request( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );
    void on_response(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from );
};

#endif // ROTATORLOGIC_H
