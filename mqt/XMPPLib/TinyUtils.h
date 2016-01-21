/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#ifndef TinyUtilsH
#define TinyUtilsH

class TiXmlElement;
extern bool checkElementName( TiXmlElement *e, const QString &expected );
extern QString getAttribute( TiXmlElement *tix, const QString &attname );

#endif
