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

#include <stdexcept>
#include <QString>
class XStanza;
void makeXMPPEvent( XStanza *a );

class Exception: public std::exception
{
    QString messStr;
public:
    Exception(const char *s):messStr(s)
    {

    }
    virtual ~Exception() override;
    QString mess()
    {
        return messStr;
    }
};

//---------------------------------------------------------------------------
#endif
