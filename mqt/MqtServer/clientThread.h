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
//==============================================================================

class MinosClientConnection: public MinosCommonConnection
{
    //Q_OBJECT
   private:
   protected:
   public:
      MinosClientConnection();
      virtual bool initialise( bool conn) override;
      ~MinosClientConnection();
      virtual bool checkFrom( TiXmlElement *pak );
      virtual void setFromId( MinosId &from, RPCRequest *req );
      virtual bool isServer()
      {
         return false;
      }
      virtual QString getIdentity()
      {
         return "MinosClientConnection " + makeJid();
      }
      void closeDown() override;
};
//==============================================================================

#endif
