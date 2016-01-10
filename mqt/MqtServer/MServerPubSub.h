/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#include "minos_pch.h"

#ifndef MServerPubSubH
#define MServerPubSubH 
//---------------------------------------------------------------------------
class MinosRPCObj;
class TPubSubMain
{
   private:  	// User declarations
      void logMessage( QString s );
   public:  		// User declarations
      TPubSubMain( );
      ~TPubSubMain( );
      void publishCallback( bool err, MinosRPCObj *mro, const QString &from );
      void subscribeCallback( bool err, MinosRPCObj *mro, const QString &from );
      void remoteSubscribeCallback( bool err, MinosRPCObj *mro, const QString &from );
      void serverSubscribeCallback( bool err, MinosRPCObj *mro, const QString &from );
      void notifyCallback( bool err, MinosRPCObj *mro, const QString &from );
      void serverNotifyCallback( bool err, MinosRPCObj *mro, const QString &from );

      bool publish( const QString &pubId, const QString &category, const QString &key, const QString &value, PublishState state );
      bool serverPublish( const QString &pubId, const QString &svr, const QString &category, const QString &key, const QString &value, PublishState state );
      bool analyseNode( TiXmlElement *pak );

      void revokeClient(const QString &pubId);
      void disconnectServer(const QString &pubId);
      void closeDown();

};
extern TPubSubMain *PubSubMain;
#endif
