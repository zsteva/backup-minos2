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

signals:

public slots:


private slots:
    void on_notify(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from );
    void on_request( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );
    void on_response(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from );

};

#endif // RIGCONTROLRPC_H
