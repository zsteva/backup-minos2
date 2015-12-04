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

bool checkElementName( TiXmlElement *e, const QString &expected )
{
   if ( expected.compare(e->Value(), Qt::CaseInsensitive) == 0 )
      return true;
   return false;
}

QString getAttribute( TiXmlElement *tix, const QString &attname )
{
   QString attval;
   const char * att = tix->Attribute( attname.toStdString().c_str() );
   if ( att )
   {
      attval = QString(att);
   }
   return attval;
}

