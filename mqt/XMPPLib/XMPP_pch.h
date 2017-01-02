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

#include <cstdint>

#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QSettings>
#include <QSharedMemory>
#include <QTextStream>
#include <QThread>
#include <QSystemSemaphore>
#include <QTcpSocket>
#include <QSettings>
#include <QSharedPointer>

#include <exception>
#include <string>
#include <algorithm>    // and it also gets included by a different route

#include <stdio.h>
#include <errno.h>

#include "tinyxml.h"
#include "TinyUtils.h"

#include "LogEvents.h"
#include "MTrace.h"
#include "MLogFile.h"

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
#endif
