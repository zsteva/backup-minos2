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
int GetStringAttribute( TiXmlElement *e, const QString &name, QString &s )
{
   const TIXML_STRING * a = e->Attribute( name.toStdString() );// allowed conversion through TIXML_STRING
   if ( !a )
   {
      return TIXML_NO_ATTRIBUTE;
   }
   s = QString::fromStdString(*a);

   return TIXML_SUCCESS;
}
int GetStringAttribute( TiXmlElement *e, const QString &name, QString &s, const QString &def )
{
   int ret = GetStringAttribute( e, name, s );
   if ( ret == TIXML_NO_ATTRIBUTE )
   {
      s = def;
   }
   return ret;
}
int GetIntAttribute( TiXmlElement *e, const QString &name, int &i )
{
   const TIXML_STRING * a = e->Attribute( name.toStdString() );// allowed conversion through TIXML_STRING
   if ( !a )
   {
      i = 0;
      return TIXML_NO_ATTRIBUTE;
   }

   char *endpt = 0;
   unsigned long l = strtoul( ( *a ).c_str(), &endpt, 0 );

   i = l;

   if ( endpt == ( *a ).c_str() )
      return TIXML_NO_ATTRIBUTE;

   return TIXML_SUCCESS;
}
int GetIntAttribute( TiXmlElement *e, const QString &name, int &i, int def )
{
   int ret = GetIntAttribute( e, name, i );
   if ( ret == TIXML_NO_ATTRIBUTE )
   {
      i = def;
   }
   return ret;
}
int GetBoolAttribute( TiXmlElement *e, const QString &name, bool &b )
{
   QString val;
   int ret = GetStringAttribute( e, name, val );
   if ( ret == TIXML_SUCCESS )
   {
      if ( val[ 0 ] == 't' || val[ 0 ] == 'T' || val[ 0 ] == 'y' || val[ 0 ] == 'Y' || val[ 0 ] == '1' )
      {
         b = true;
      }
      else
      {
         b = false;
      }
   }
   return ret;
}
int GetBoolAttribute( TiXmlElement *e, const QString &name, bool &b, bool def )
{
   int ret = GetBoolAttribute( e, name, b );
   if ( ret == TIXML_NO_ATTRIBUTE )
   {
      b = def;
   }
   return ret;
}
