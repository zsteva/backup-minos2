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

#ifndef serverThreadH
#define serverThreadH
//---------------------------------------------------------------------------
extern GJV_thread *serverThread;
extern void runServerThread( void * );
class Server;
//==============================================================================
class MinosServerListener: public MinosListener
{
   private:
      static MinosServerListener *MSL;
   protected:
   public:
      static MinosServerListener *getListener()
      {
         return MSL;
      }
      MinosServerListener()
      {
         MSL = this;
      }
      ~MinosServerListener() override
      {
         MSL = 0;
      }
//      virtual void process() override;
      bool sendServer( MinosCommonConnection *il, TiXmlElement *pak );
      void checkServerConnected( Server *s, bool force );
      bool checkStillServerConnection( const QString &s );
      virtual QString getIdentity() override
      {
         return "MinosServerListener";
      }
};
//==============================================================================
class MinosServerConnection: public MinosCommonConnection
{
   private:
      Server *srv;
      QDateTime lastEventTick;
      bool resubscribed;
   protected:
   public:
      MinosServerConnection();
      virtual bool initialise();
      ~MinosServerConnection();
      virtual bool checkFrom( TiXmlElement *pak );
      virtual bool isServer()
      {
         return true;
      }
      virtual bool setFromId( MinosId &from, RPCRequest *req );

      bool ioConnect ( const QString &host, int port );
      bool connectWith ( const QString &host, int port );
      virtual bool mConnect( Server *srv );
      virtual void sendAction( XStanza *a );
      virtual QString getIdentity()
      {
         return "MinosServerConnection " + makeJid();
      }
      virtual void sendKeepAlive( );
};
//==============================================================================
#endif
