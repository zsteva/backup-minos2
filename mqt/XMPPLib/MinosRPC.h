#ifndef MINOSRPC_H
#define MINOSRPC_H

#include "XMPP_pch.h"
#include <QTimer>

class RPCGeneralClient: public MinosRPCClient
{
   public:
    RPCGeneralClient( QString methodName) : MinosRPCClient( methodName, 0, true )
    {}
      RPCGeneralClient( TRPCFunctor *cb ) : MinosRPCClient( "", cb, true )
      {}
      ~RPCGeneralClient()
      {}
      virtual QSharedPointer<MinosRPCObj> makeObj()
      {
         return QSharedPointer<MinosRPCObj>(new RPCGeneralClient( callback ));
      }
};
class RPCGeneralServer: public MinosRPCServer
{
   public:
    RPCGeneralServer( ) : MinosRPCServer( "", 0, true )
    {}
      RPCGeneralServer( TRPCFunctor *cb ) : MinosRPCServer( "", cb, true )
      {}
      ~RPCGeneralServer()
      {}
      virtual QSharedPointer<MinosRPCObj>makeObj()
      {
         return QSharedPointer<MinosRPCObj>(new RPCGeneralServer( callback ));
      }

};

class MinosRPC: public QObject
{
    Q_OBJECT

    MinosRPC(const QString &defaultName, bool useEnvVar);

    static MinosRPC *rpc;

    QString appName;
    bool connected;
    bool subscribed;

    QTimer connectTimer;

    QList <QString> subscriptions;
    QList <QPair <QString, QString> > remoteSubscriptions;

    void setAppName(const QString &);

    void notifyCallback( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );
    void clientCallback( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );
    void serverCallback( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );

public:

    static MinosRPC *getMinosRPC(QString defaultName = QString(), bool useEnvVar = true)
    {
        if (!rpc && !defaultName.isEmpty())
        {
            rpc = new MinosRPC(defaultName, useEnvVar);
        }
        return rpc;
    }

    void subscribe(const QString &);
    void subscribeRemote(const QString &, const QString &);

    void publish( const QString &category, const QString &key, const QString &value, PublishState pState );


signals:
    void notify( bool err, QSharedPointer<MinosRPCObj>, const QString &from );
    void clientCall( bool err, QSharedPointer<MinosRPCObj>, const QString &from );
    void serverCall( bool err, QSharedPointer<MinosRPCObj>, const QString &from );
private slots:
    void on_connectedTimeout();
};

#endif // MINOSRPC_H
