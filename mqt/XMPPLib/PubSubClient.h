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

#ifndef PubSubClientH
#define PubSubClientH 
//---------------------------------------------------------------------------
class RPCPublishClient: public RPCPubSub
{
   public:
      RPCPublishClient( TRPCFunctor *cb ) : RPCPubSub( "Minos:PubSub:Publish", cb )
      {}    // method name should be shared
      // plus we want factory access to it
      // so it would be nice to have a static
      // member/method  that could go "up"
      ~RPCPublishClient()
      {}
      virtual RPCPublishClient *makeObj()
      {
         return new RPCPublishClient( callback );
      }
};

// Subscribe is a call - the subcribed thing might not be there
class RPCSubscribeClient: public RPCPubSub
{
   public:
      RPCSubscribeClient( TRPCFunctor *cb ) : RPCPubSub( "Minos:PubSub:Subscribe", cb )
      {}
      ~RPCSubscribeClient()
      {}
      virtual RPCSubscribeClient *makeObj()
      {
         return new RPCSubscribeClient( callback );
      }
};
class RPCRemoteSubscribeClient: public RPCPubSub
{
   public:
      RPCRemoteSubscribeClient( TRPCFunctor *cb ) : RPCPubSub( "Minos:PubSub:RemoteSubscribe", cb )
      {}
      ~RPCRemoteSubscribeClient()
      {}
      virtual RPCRemoteSubscribeClient *makeObj()
      {
         return new RPCRemoteSubscribeClient( callback );
      }
};
class RPCNotifyServer: public MinosRPCServer
{
   public:
      RPCNotifyServer( TRPCFunctor *cb ) : MinosRPCServer( "Minos:PubSub:ClientNotify", cb )
      {}
      ~RPCNotifyServer()
      {}
      virtual RPCNotifyServer *makeObj()
      {
         return new RPCNotifyServer( callback );
      }
};
//---------------------------------------------------------------------------
class RPCSubscriber
{
   protected:
      QString category;
      RPCSubscriber( const QString &category )
            : category( category )
      {}
   public:
      static void testAndSubscribe( const QString &category );
      virtual ~RPCSubscriber(){}
      virtual void reSubscribe();
      virtual bool isEqual( const QString &category );
      virtual bool isRemoteEqual( const QString &server, const QString &category );
};
class RPCRemoteSubscriber : public RPCSubscriber
{
   private:
      QString server;
      RPCRemoteSubscriber( const QString &server, const QString &category )
            : RPCSubscriber( category ), server( server )
      {}
   public:
      static void testAndSubscribe( const QString &server, const QString &category );
      virtual bool isRemoteEqual( const QString &server, const QString &category );
      virtual void reSubscribe();
      QString getServer()
      {
         return server;
      }
};
class RPCPublisher
{
      QString category;
      QString key;
      QString value;
      PublishState state;
      RPCPublisher( const QString &category, const QString &key, const QString &value, PublishState pState )
            : category( category ), key( key ), value( value ), state(pState)
      {}
   public:
      static void testAndPublish( const QString &category, const QString &key, const QString &value, PublishState pState );
      void rePublish();

};
extern std::vector<RPCSubscriber *> subscribeList;
extern std::vector<RPCPublisher *> publishList;
#endif