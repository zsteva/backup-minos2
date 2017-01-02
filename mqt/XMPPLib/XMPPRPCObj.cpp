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

QMap <QString, QSharedPointer<MinosRPCObj>> &getServerMethodMap()
{
   static QMap <QString, QSharedPointer<MinosRPCObj> > serverMethodMap;
   return serverMethodMap;
}
QMap <QString, QSharedPointer<MinosRPCObj> > &getClientMethodMap()
{
   static QMap <QString, QSharedPointer<MinosRPCObj> > clientMethodMap;
   return clientMethodMap;
}
//==============================================================================
MinosRPCObj::MinosRPCObj(const QString &methodName, TRPCFunctor *cb , bool gen)
      : methodName( methodName ), callback( cb ), general(gen)
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
   for ( QMap <QString, QSharedPointer<MinosRPCObj> >::iterator i = getServerMethodMap().begin(); i != getServerMethodMap().end(); i++ )
   {
      delete i.value()->callback;
      i.value().reset();
   }
   getServerMethodMap().clear();
   for ( QMap <QString, QSharedPointer<MinosRPCObj> >::iterator i = getClientMethodMap().begin(); i != getClientMethodMap().end(); i++ )
   {
       delete i.value()->callback;
       i.value().reset();
   }
   getClientMethodMap().clear();
}

/*static*/ void MinosRPCObj::addClientObj(QSharedPointer<MinosRPCObj> mro )
{
   getClientMethodMap().insert(  mro->methodName, mro  );
}
/*static*/ void MinosRPCObj::addServerObj(  QSharedPointer<MinosRPCObj> mro )
{
   getServerMethodMap().insert( mro->methodName, mro );
}

/*static*/ QSharedPointer<MinosRPCObj> MinosRPCObj::makeClientObj(  QString call )
{
   QMap <QString, QSharedPointer<MinosRPCObj> >::iterator mo = getClientMethodMap().find( call );
   if ( mo != getClientMethodMap().end() )
   {
      return mo.value()->makeObj();
   }
   for (mo = getClientMethodMap().begin(); mo != getClientMethodMap().end(); mo++)
   {
       if (mo.value()->isGeneralObject())
       {
           return mo.value()->makeObj();
       }
   }
   return QSharedPointer<MinosRPCObj>();
}

/*static*/ QSharedPointer<MinosRPCObj> MinosRPCObj::makeServerObj(  QString call )
{
   QMap <QString, QSharedPointer<MinosRPCObj> >::iterator mo = getServerMethodMap().find( call );
   if ( mo != getServerMethodMap().end() )
   {
      return mo.value()->makeObj();
   }
   for (mo = getServerMethodMap().begin(); mo != getServerMethodMap().end(); mo++)
   {
       if (mo.value()->isGeneralObject())
       {
           return mo.value()->makeObj();
       }
   }
   return QSharedPointer<MinosRPCObj>();
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

