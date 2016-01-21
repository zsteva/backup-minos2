/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#include "XMPP_pch.h"

extern void sendAction( XStanza *a );      // which might not be the one in XMPPThread

//---------------------------------------------------------------------------

std::map <QString, MinosRPCServer *> &getServerMethodMap()
{
   static std::map <QString, MinosRPCServer *> serverMethodMap;
   return serverMethodMap;
}
std::map <QString, MinosRPCClient *> &getClientMethodMap()
{
   static std::map <QString, MinosRPCClient *> clientMethodMap;
   return clientMethodMap;
}
//==============================================================================
MinosRPCObj::MinosRPCObj( const QString &methodName, TRPCFunctor *cb )
      : methodName( methodName ), callback( cb )
{}
MinosRPCObj::~MinosRPCObj()
{
   //delete callArgs;
}
void MinosRPCObj::clearCallArgs()
{
   callArgs.args.clear();  // clears shared pointers
}
/*static*/ void MinosRPCObj::clearRPCObjects()
{
   for ( std::map <QString, MinosRPCServer *>::iterator i = getServerMethodMap().begin(); i != getServerMethodMap().end(); i++ )
   {
      delete ( *i ).second->callback;
      delete ( *i ).second;
   }
   getServerMethodMap().clear();
   for ( std::map <QString, MinosRPCClient *>::iterator i = getClientMethodMap().begin(); i != getClientMethodMap().end(); i++ )
   {
      delete ( *i ).second->callback;
      delete ( *i ).second;
   }
   getClientMethodMap().clear();
}

/*static*/ void MinosRPCObj::addObj(  MinosRPCClient *mro )
{
   getClientMethodMap().insert( std::pair <QString, MinosRPCClient *>( mro->methodName, mro ) );
}
/*static*/ void MinosRPCObj::addObj(  MinosRPCServer *mro )
{
   getServerMethodMap().insert( std::pair <QString, MinosRPCServer *>( mro->methodName, mro ) );
}

/*static*/ MinosRPCClient *MinosRPCObj::makeClientObj(  QString call )
{
   std::map <QString, MinosRPCClient *>::iterator mo = getClientMethodMap().find( call );
   if ( mo != getClientMethodMap().end() )
      return ( *mo ).second->makeObj();
   return 0;
}

/*static*/ MinosRPCServer *MinosRPCObj::makeServerObj(  QString call )
{
   std::map <QString, MinosRPCServer *>::iterator mo = getServerMethodMap().find( call );
   if ( mo != getServerMethodMap().end() )
      return ( *mo ).second->makeObj();
   return 0;
}
//==============================================================================
void MinosRPCClient::queueCall( QString to )
{
   RPCRequest * m = new RPCRequest( to, methodName );
   m->setNextId();      // only happens if no Id already
   m->args = callArgs.args;     // copy vector of pointers
   sendAction( m );
   delete m;
}
//==============================================================================
// server...
void MinosRPCServer::queueResponse( QString to )
{
   RPCResponse * m = new RPCResponse( to, id, methodName );
   m->args = callArgs.args;     // copy vector of pointers
   sendAction( m );
   delete m;
}
void MinosRPCServer::queueErrorResponse( QString to )
{
   RPCResponse * m = new RPCResponse( to, id, methodName );
   sendAction( m );
   delete m;
}
//==============================================================================

