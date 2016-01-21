/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////

#ifndef DispatcherH
#define DispatcherH

class TiXmlElement;
class XStanza;
class RPCDispatcher
{
   public:
      RPCDispatcher()
      {}
      virtual ~RPCDispatcher()
      {}
      virtual void dispatchResponse( XStanza *a ) = 0;

};
extern bool analyseNode( RPCDispatcher *user_data, TiXmlElement *e );
extern bool analyseNode( RPCDispatcher *user_data, TIXML_STRING UTF8XML );
TiXmlElement *findNode( TiXmlElement *node, const QString &name );
QString getNodeValue( TiXmlElement *node, const QString &name );
QString getAttribute(TiXmlElement *tix, const QString &attname );
#endif
