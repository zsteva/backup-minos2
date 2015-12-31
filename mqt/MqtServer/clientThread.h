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

#ifndef clientThreadH
#define clientThreadH 

#include "MinosLink.h"
//---------------------------------------------------------------------------
extern GJV_thread *clientThread;
extern void runClientThread( void * );
//==============================================================================
class MinosClientListener: public MinosListener
{
   private:
      static MinosClientListener *MCL;
   protected:
   public:
      static MinosClientListener *getListener()
      {
         return MCL;
      }
      MinosClientListener()
      {
         MCL = this;
      }
      ~MinosClientListener() override
      {
         MCL = 0;
      }
//      virtual void process() override;
      bool sendClient( MinosCommonConnection *il, TiXmlElement *pak );
      virtual QString getIdentity()
      {
         return "MinosClientListener";
      }
      bool checkStillClientConnection( const QString &s );
};
//==============================================================================
class MinosClientConnection: public MinosCommonConnection
{
   private:
   protected:
   public:
      MinosClientConnection();
      virtual bool initialise();
      ~MinosClientConnection();
      virtual bool checkFrom( TiXmlElement *pak );
      virtual bool setFromId( MinosId &from, RPCRequest *req );
      virtual bool isServer()
      {
         return false;
      }
      virtual QString getIdentity()
      {
         return "MinosClientConnection " + makeJid();
      }
};
//==============================================================================

#endif
