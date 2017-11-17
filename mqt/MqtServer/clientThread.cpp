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

#include "MinosLink.h"
#include "clientThread.h"
#include "serverThread.h"

//==============================================================================
//==============================================================================

MinosClientConnection::MinosClientConnection()
{
}
bool MinosClientConnection::initialise(bool conn)
{
   connected = conn;
   QHostAddress h = sock->peerAddress();
   connectHost = h.toString();
   connect(sock.data(), SIGNAL(readyRead()), this, SLOT(on_readyRead()));
   connect(sock.data(), SIGNAL(disconnected()), this, SLOT(on_disconnected()));

   return true;
}

MinosClientConnection::~MinosClientConnection()
{
}
//==============================================================================
void MinosClientConnection::closeDown()
{
    logMessage( "Client Link", "Closing" );
    // here we need to revoke all of this clients published keys
    PubSubMain->revokeClient(makeJid());
}
//==============================================================================
bool MinosClientConnection::checkFrom( TiXmlElement *tix )
{
   if ( !fromIdSet )
   {
      return true;
   }
   // if no "from" insert one

   const char *sfrom = tix->Attribute( "from" );
   if ( !sfrom )
   {
      // insert a from of ourselves

      QString from = makeJid();
      if ( from.size() )
      {
         tix->SetAttribute( "from", from.toStdString().c_str() );
      }
   }
   else
   {
      MinosId from( sfrom );
      if ( !checkServer( from ) )
         return false;

      if ( !checkUser( from ) )
         return false;
   }
   return true;
}
//==============================================================================
void MinosClientConnection::setFromId( MinosId &from, RPCRequest * /*req*/ )
{
   // this should always be a local client
   clientServer = from.server;
   clientUser = from.user;
   fromIdSet = true;
}
//==============================================================================

