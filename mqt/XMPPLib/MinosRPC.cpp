#include <QProcessEnvironment>
#include "MinosRPC.h"
#include "ServerEvent.h"
#include "MinosConnection.h"
#include "MTrace.h"
/*static*/ MinosRPC *MinosRPC::rpc = nullptr;

RPCGeneralClient::~RPCGeneralClient()
{}
RPCGeneralServer::~RPCGeneralServer()
{}

MinosRPC::MinosRPC(const QString &defaultName, bool useEnvVar):
    connected(false), subscribed(false)
{
    QString rpcName = defaultName;
    if (useEnvVar)
    {
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        rpcName = env.value("MQTRPCNAME", defaultName);

        trace("Value of MQTRPCNAME for " + defaultName + " is " + rpcName);
    }
    setAppName(rpcName);

    connect(&connectTimer, SIGNAL(timeout()), this, SLOT(on_connectedTimeout()));
    connectTimer.start(100);
}
void MinosRPC::on_connectedTimeout()
{
    if ( !connected && checkServerReady() )
    {
        MinosRPCObj::addServerObj( QSharedPointer<MinosRPCObj>(new RPCGeneralServer( new TRPCCallback <MinosRPC> ( this, &MinosRPC::serverCallback ) ) ) );
        XMPPInitialise( appName );
        connected = true;
    }
    if ( connected && !subscribed )
    {
        RPCPubSub::initialisePubSub( new TRPCCallback <MinosRPC> ( this, &MinosRPC::notifyCallback ) );

        QPair<QString, QString> p;
        foreach (p, remoteSubscriptions)
        {
            RPCPubSub::subscribeRemote( p.first, p.second );
        }

        foreach (QString c, subscriptions)
        {
            RPCPubSub::subscribe( c );
        }

        subscribed = true;
    }
}

void MinosRPC::setAppName(const QString &n)
{
    appName = n;
}
QString MinosRPC::getAppName()
{
    return appName;
}


void MinosRPC::subscribe(const QString &c)
{
    subscriptions.append(c);
    if (subscribed)
    {
        RPCPubSub::subscribe( c );
    }
}
void MinosRPC::subscribeRemote(const QString &s, const QString &c)
{
    remoteSubscriptions.append(QPair<QString, QString>(s, c));
    if (subscribed)
    {
        RPCPubSub::subscribeRemote( s, c );
    }
}
void MinosRPC::publish( const QString &category, const QString &key, const QString &value, PublishState pState )
{
   RPCPubSub::publish( category, key, value, pState );
}
void MinosRPC::notifyCallback( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
{
   trace( "Notify callback from " + from + ( err ? ":Error" : ":Normal" ) );

    emit notify(err, mro, from);
}

//---------------------------------------------------------------------------
void MinosRPC::serverCallback( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
{
   trace( "rpc server callback from " + from + ( err ? ":Error" : ":Normal" ) );

   emit serverCall(err, mro, from);
}
