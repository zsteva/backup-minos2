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
    std::string messStr;
public:
    Exception(const char *s):messStr(s)
    {

    }
    std::string mess()
    {
        return messStr;
    }
};

//---------------------------------------------------------------------------
#endif