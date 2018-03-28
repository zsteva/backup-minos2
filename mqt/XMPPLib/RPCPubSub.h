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
#include "XMPPRPCObj.h"
//---------------------------------------------------------------------------
enum    PublishState {psPublished, psRevoked, psNotConnected};

class RPCPubSub : public MinosRPCClient
{
   protected:
      static bool connected;
      RPCPubSub( const QString &call, TRPCFunctor *cb ) : MinosRPCClient( call, cb )
      {}
      virtual ~RPCPubSub() override;

      virtual QSharedPointer<MinosRPCObj>makeObj() override = 0;
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
      QString publisherProgram;
      QString publisherServer;
      QString server;
      QString category;
      QString key;
      QString value;
      PublishState state;

      bool OK;
   public:
      AnalysePubSubNotify( bool err, QSharedPointer<MinosRPCObj>mro );
      QString getPublisherProgram() const
      {
         return publisherProgram;
      }
      QString getPublisherServer() const
      {
         return publisherServer;
      }
      QString getServer() const
      {
         return server;
      }
      QString getCategory() const
      {
         return category;
      }
      QString getKey() const
      {
         return key;
      }
      QString getValue() const
      {
         return value;
      }
      PublishState getState() const
      {
         return state;
      }
      bool getOK() const
      {
         return OK;
      }

};
//---------------------------------------------------------------------------
#endif
