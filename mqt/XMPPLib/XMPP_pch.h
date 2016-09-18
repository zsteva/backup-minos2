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

#include <stdint.h>

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

#include <memory>
#include <exception>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <algorithm>
#include <sstream>
#include <fstream>

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
template < class qe >
class my_deque : public std::deque < qe >
{
   public:
      my_deque()
      {}
      ~my_deque()
      {}

      void freeAll()
      {
         //disableInterrupts fred;
          for (typename  my_deque::iterator i = this->begin(); i != this->end(); i++ )
            delete ( *i );
          my_deque::clear();
      }

      qe find( const QString &s )
      {
         //disableInterrupts fred;
         for (typename  my_deque::iterator i = this->begin(); i != this->end(); i++ )
            if ( ( *i ) ->pName.compare(s, Qt::CaseInsensitive ) == 0 )
               return ( *i );
         return 0;
      }

      void clear_after ( qe e )
      {
         //disableInterrupts fred;
         typename my_deque::iterator i = std::find( this->begin(), this->end(), e );
         if ( i == this->end() )
            return ;
         i++;
         if ( i == this->end() )
            return ;
         for ( typename my_deque::iterator i2 = i; i2 != this->end(); i2++ )
         {
            delete ( *i2 );
         }
         my_deque::erase( i, this->end() );
      }
      void free_element ( qe e )
      {
         //disableInterrupts fred;
         if ( e )
         {
            typename my_deque::iterator i = std::find( this->begin(), this->end(), e );
            if ( i != this->end() )
            {
               my_deque::erase( i );
            }
            delete e;
         }
      }

      qe next_element( qe q )
      {
         //disableInterrupts fred;
         typename my_deque::iterator i = std::find( this->begin(), this->end(), q );
         if ( i != this->end() )
         {
            i++;
            if ( i != this->end() )
               return * i;
         }
         return 0;
      }
};
#endif
