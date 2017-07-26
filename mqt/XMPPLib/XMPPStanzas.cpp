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
#include "tinyxml.h"
#include "TinyUtils.h"
//---------------------------------------------------------------------------
// Basic classes
// base of actions to be passed to XMPP

XStanza::XStanza()
{}
XStanza::~XStanza()
{}
void XStanza::setNextId()
{
   if ( id.size() == 0 )
   {
      id = getNextId();
   }
}
/*static*/ QString XStanza::getNextId()
{
   static int nextId = 1;
   static char buff[ 40 ] = {0};

   sprintf( buff, "%d", nextId );
   nextId++;

   return buff;
}
//---------------------------------------------------------------------------
// RPC actions. NB that calls, events and responses can come from either end!
//---------------------------------------------------------------------------

// RPC base action
RPCAction::RPCAction()
{}
RPCAction::RPCAction( const QString &to, const QString &from ) :
      to( to ), from( from )
{}
RPCAction::~RPCAction()
{
   args.clear();
}
QString RPCAction::print()
{
   QString s = "From: " + getFrom() + " To: " + getTo() + "\r\n";
   for ( QVector<QSharedPointer<RPCParam> >::iterator i = args.begin(); i != args.end(); i++ )
   {
      s += ( *i ) ->print();
   }
   return s;
}
void RPCAction::parseParams( TiXmlElement *paramsNode )      // parse from the node to args
{
   TIXML_STRING s;
   s << *paramsNode ;
   RPCArgs::parseParams( s );
}

//---------------------------------------------------------------------------
// Do an RPC action (normally the receiver!)
TiXmlElement *findNode( TiXmlElement *node, const QString &name )
{
   for ( TiXmlElement * e = node->FirstChildElement(); e; e = e->NextSiblingElement() )
   {
      // declaration, destination lines
      if ( checkElementName( e, name ) )
      {
         return e;
      }
   }
   return 0;
}
QString getNodeValue( TiXmlElement *node, const QString &name )
{
   QString res;
   for ( TiXmlElement * e = node->FirstChildElement(); e; e = e->NextSiblingElement() )
   {
      // declaration, destination lines
      if ( checkElementName( e, name ) )
      {
         const char * val = e->GetText();
         if ( val )
         {
            res = val;
         }
         break;
      }
   }
   return res;
}
RPCRequest::RPCRequest( const QString &to, const QString &mname ) : RPCAction( to, "" ), methodName( mname )
{}
RPCRequest::RPCRequest( const QString &to, const QString &from, const QString &mname ) : RPCAction( to, from ), methodName( mname )
{}
RPCRequest::RPCRequest( const QString &from, TiXmlElement *node ) : RPCAction( "", from )
{
   QString mname = getNodeValue( node, "methodName" );
   methodName = mname;
   TiXmlElement *p = findNode( node, "params" );
   if ( p )
   {
      parseParams( p );
   }
}
RPCRequest::~RPCRequest()
{}

TiXmlElement *makeIq ( const QString &type, const QString &xmlns )
{
   TiXmlElement * x = new TiXmlElement( "iq" );
   x->SetAttribute( "type", type.toStdString() );
   TiXmlElement qNode( "query" );
   qNode.SetAttribute( "xmlns", xmlns.toStdString() );

   x->InsertEndChild( qNode );
   return x;
}

// Build up the DOM tree for the action, and send it
TIXML_STRING RPCRequest::getActionMessage( )
{
   TiXmlElement * x = makeIq( "set", "minos:iq:rpc" );
   if ( to.length() )
      x->SetAttribute( "to", to.toStdString() );
   if ( from.length() )
      x->SetAttribute( "from", from.toStdString() );
   if ( getId().length() )
      x->SetAttribute( "id", getId().toStdString() );

   TiXmlElement *q = findNode( x, "query" );

   TiXmlElement mcNode( "methodCall" );
   TiXmlElement mNode( "methodName" );

   TiXmlText tNode( methodName.toStdString() );
   mNode.InsertEndChild( tNode );

   mcNode.InsertEndChild( mNode );
   addParams( mcNode );
   q->InsertEndChild( mcNode );

   TIXML_STRING s;
   s << *x ;
   delete x;
   return s;
}
QString RPCRequest::print()
{
   QString s = "Request\r\n";
   s += RPCAction::print();
   return s;
}
QString RPCRequest::analyse()
{
   QString s;
   for ( QVector<QSharedPointer<RPCParam> >::iterator i = args.begin(); i != args.end(); i++ )
   {
      s += ( *i ) ->analyse();
   }
   return s;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Do an RPC action (normally the receiver!)
RPCResponse::RPCResponse( const QString &to, const QString &pid , const QString &mname ) :
      RPCAction( to, "" ), methodName( mname )
{
   setId( pid );
}
RPCResponse::RPCResponse( const QString &to, const QString &from, const QString &pid , const QString &mname ) :
      RPCAction( to, from ), methodName( mname )
{
   setId( pid );
}
RPCResponse::RPCResponse( const QString &from, TiXmlElement *node ) :
      RPCAction( "", from )
{
   // node points to methodResponse
   methodName = getNodeValue( node, "methodName" );

   TiXmlElement * p = findNode( node, "params" );
   if ( p )
   {
      parseParams( p );
   }
   else
   {
      TiXmlElement *mf = findNode( node, "fault" );
      if ( mf )
      {
         TiXmlElement * mv = findNode( mf, "value" );
         if ( mv )
         {
            TiXmlElement * ms = findNode( mv, "struct" );
            if ( ms )
            {
               fault = QSharedPointer<RPCParam>(new RPCParamStruct( *ms ));
            }
         }
      }
   }
}
RPCResponse::RPCResponse()
{}
RPCResponse::~RPCResponse()
{
}
void RPCResponse::addFault( QSharedPointer<RPCParam>p )
{
   fault = p;
}
void RPCResponse::addFault( TiXmlElement &node )
{
   if ( fault )
   {
      TiXmlElement pNode( "fault" );
      fault->addValue( pNode );
      RPCArgs xm;
      xm.addParam( fault );
      TiXmlElement *pnode = xm.makeParamsNode();
      node.InsertEndChild( *pnode );
      delete pnode;
   }
}
// Build up the DOM tree for the action, and send it
TIXML_STRING RPCResponse::getActionMessage( )
{
   TiXmlElement * x = makeIq( "result", "minos:iq:rpc" );
   if ( to.length() )
      x->SetAttribute( "to", to.toStdString() );
   if ( from.length() )
      x->SetAttribute( "from", from.toStdString() );
   if ( getId().length() )
      x->SetAttribute( "id", getId().toStdString() );

   TiXmlElement *q = findNode( x, "query" );

   TiXmlElement mrNode( "methodResponse" );
   TiXmlElement mNode( "methodName" );
   TiXmlText tNode( methodName.toStdString() );
   mNode.InsertEndChild( tNode );
   mrNode.InsertEndChild( mNode );

   if ( fault )
      addFault( mrNode );
   else
      addParams( mrNode );
   q->InsertEndChild( mrNode );

   TIXML_STRING s;
   s << *x;
   delete x;
   return s;
}
QString RPCResponse::print()
{
   QString s = "Response\r\n";
   if ( fault )
      s += fault->print();

   s += RPCAction::print();
   return s;
}
//---------------------------------------------------------------------------


