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

#ifndef XMPPEventsH
#define XMPPEventsH
class XStanza;
void makeXMPPEvent( XStanza *a );

class Exception: public std::exception
{
    QString messStr;
public:
    Exception(const char *s):messStr(s)
    {

    }
    QString mess()
    {
        return messStr;
    }
};

//---------------------------------------------------------------------------
#endif
