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

#ifndef XMPPStanzasH
#define XMPPStanzasH 
//---------------------------------------------------------------------------
// Basic classes
// base of actions to be passed to XMPP


class XStanza
{
      QString id;
   public:

      XStanza();
      virtual ~XStanza();

      virtual void setId( const QString &tid )
      {
         id = tid;
      }
      virtual QString getId()
      {
         return id;
      }
      void setNextId();
      static QString getNextId();

      virtual QString getTo() = 0;
      virtual QString getFrom() = 0;

      // Build up the DOM tree for the action, and send it
      virtual TIXML_STRING getActionMessage() = 0;

      virtual QString print() = 0;
      virtual QString analyse() = 0;
};
//---------------------------------------------------------------------------
// RPC actions. NB that calls and events can come from either end!

// RPC base action
class RPCAction: public XStanza, public RPCArgs
{
   protected:
      void parseParams( TiXmlElement *paramsNode );     // parse from the node to args
      QString to;
      QString from;
   public:

      QString getTo()
      {
         return to;
      }
      QString getFrom()
      {
         return from;
      }

      RPCAction();
      RPCAction( const QString &to, const QString &from );
      virtual ~RPCAction();
      // Build up the DOM tree for the action, and send it
      virtual TIXML_STRING getActionMessage() = 0;
      virtual QString print() = 0;
      virtual QString analyse()
      {
         return "";
      }
};
// Do an RPC action (normally the receiver!)
class RPCRequest: public RPCAction
{
   public:
      QString methodName;
      RPCRequest( const QString &to, const QString &mname );
      RPCRequest( const QString &to, const QString &from, const QString &mname );
      RPCRequest( const QString &from, TiXmlElement *node );
      virtual ~RPCRequest();
      // Build up the DOM tree for the action, and send it
      virtual TIXML_STRING getActionMessage( );
      virtual QString print();
      virtual QString analyse();
};

//---------------------------------------------------------------------------
#endif
