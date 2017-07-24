/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef RPCPubSubH
#define RPCPubSubH 
//---------------------------------------------------------------------------
enum    PublishState {psPublished, psRevoked, psNotConnected};

class RPCPubSub : public MinosRPCClient
{
   protected:
      static bool connected;
      RPCPubSub( const QString &call, TRPCFunctor *cb ) : MinosRPCClient( call, cb )
      {}
      virtual ~RPCPubSub()
      {}
      virtual QSharedPointer<MinosRPCObj>makeObj() = 0;
   public:
      static bool isConnected()
      {
         return connected;
      }

      static void subscribe( const QString &category );
      static void subscribeRemote( const QString &server, const QString &category );

      static void publish( const QString &category, const QString &key, const QString &value, PublishState pState );

      static void initialisePubSub( TRPCFunctor *notifycb );
      static void reconnectPubSub( );
      static void close( );
};

class AnalysePubSubNotify
{
      QString publisher;
      QString server;
      QString category;
      QString key;
      QString value;
      PublishState state;

      bool OK;
   public:
      AnalysePubSubNotify( bool err, QSharedPointer<MinosRPCObj>mro );
      QString getPublisher()
      {
         return publisher;
      }
      QString getServer()
      {
         return server;
      }
      QString getCategory()
      {
         return category;
      }
      QString getKey()
      {
         return key;
      }
      QString getValue()
      {
         return value;
      }
      PublishState getState()
      {
         return state;
      }
      bool getOK()
      {
         return OK;
      }

};
//---------------------------------------------------------------------------
#endif
