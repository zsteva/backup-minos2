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

#ifndef XMPP_pchH
#define XMPP_pchH

#include "mqtUtils_pch.h"

#include "PortIds.h"

#include "ServerEvent.h"
#include "XMPPEvents.h"

#include "Dispatcher.h"

#include "XMPPRPCParams.h"
#include "XMPPStanzas.h"
#include "XMPPRPCObj.h"
#include "RPCPubSub.h"

#include "RPCCommandConstants.h"
#include "MinosRPC.h"
#include "PubSubClient.h"

#include "MinosConnection.h"

template <class itemtype>
class MapWrapper
{
public:
    QSharedPointer<itemtype> wt;
    MapWrapper(itemtype *mp):wt(mp){}
    MapWrapper(QSharedPointer<itemtype> mp):wt(mp){}
    MapWrapper(const MapWrapper &m)
    {
        wt = m.wt;
    }
};
#endif
