/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#ifndef portconH
#define portconH 
//==============================================================================
#include "base_pch.h"

#include "keyers.h"
#include "keyctrl.h"
#include "keyconf.h"
#include "windowMonitor.h"

// forward declaration

class lineMonitor;

// This is the base class, visible to all parts of the keyer code

// It will have to contain things like the external interrupt count for CW,
// as it is port specific. Don't know how we emulate it elsewhere...

// instances of ports are created by config, and keyers and tickers register
// with them as required.

class commonPort : public QObject
{
    Q_OBJECT

   private:
   public:
      QString pName;
      QString portName;
      int openCount;

      // we need a chain of interested monitors. Only an active one should
      // actually accept any line change.
      my_deque <lineMonitor *> monitors;

      commonPort( const PortConfig &port );
      virtual ~commonPort();

      virtual bool initialise( const PortConfig &port );
      virtual bool initialisePort() = 0;

      virtual bool openPort() = 0;
      virtual bool closePort() = 0;

      virtual void ptt( int state ) = 0;
      virtual void key( int state ) = 0;

      // These "changed" calls need to link back to the controlling
      // keyer(s), which know what to do with them!

      // Ideally we should set up listeners from the keyers for
      // the lines that they are interested in on the ports they
      // use. This would have to use templates... not sure
      // how good they are in this version! Or we could just
      // register a keyer into a port, and assume that everything
      // happens on that port.

      virtual void pttChanged( int state );
      virtual void L1Changed( int state );
      virtual void L2Changed( int state );
      virtual void linesModeChanged(int lmode);

      bool lastPttState;
      bool lastL1State;
      bool lastL2State;
      int lastLinesMode;

      virtual int getlinesMode() = 0;

      virtual void checkControls( void ) = 0;

      void registerMonitor( lineMonitor * );

};

commonPort *createPort( const PortConfig &port );
extern my_deque < commonPort *> portChain;

//==============================================================================

// this is a particular implementation, created by factory and not generally seen

class WindowsMonitorPort: public commonPort
{
    Q_OBJECT
   private:
      bool PTTState;
      bool keyState;
      windowMonitor *winMonForm;
      QTimer LineCheck;
   public:
      static bool PTTInState;
      static bool L1State;
      static bool L2State;
      static int linesMode;

      static LineCallBack WinLineCallback;

      WindowsMonitorPort( const PortConfig &port );
      virtual ~WindowsMonitorPort();

      virtual bool initialisePort();

      virtual bool openPort();
      virtual bool closePort();

      virtual void ptt( int state );
      virtual void key( int state );
      virtual int getlinesMode()
      {
          return linesMode;
      }

      virtual void checkControls( void );

private slots:
      void checkWinLines();
};
class WinMonitor: public lineMonitor
{
   private:
      bool PTTState;
      bool keyState;

      bool lastPttState;
      bool lastL1State;
      bool lastL2State;
      int lastLinesMode;
   public:
      static bool PTTInState;
      static bool L1State;
      static bool L2State;
      static int linesMode;

      static LineCallBack WinLineCallback;

      WinMonitor();
      ~WinMonitor();

      virtual void ptt( int state );
      virtual void key( int state );

      virtual bool pttChanged( int state );
      virtual bool L1Changed( int state );
      virtual bool L2Changed( int state );
      virtual bool linesModeChanged(int lmode);

      virtual bool initialise( const KeyerConfig &keyer, const PortConfig &port );

      virtual void checkControls( void );
      virtual int getlinesMode()
      {
          return linesMode;
      }
};
//==============================================================================

class LineEventsPort: public commonPort
{
    Q_OBJECT
   private:
    QTimer LineTimer;
      bool PTTState;
      bool keyState;
      bool linePTTState;
      bool lineL1State;
      bool lineL2State;
      int linesMode;
      bool closing;
   public:

      bool lastPTTState;
      bool lastL1State;
      bool lastL2State;
      int lastLinesMode;

      static LineCallBack WinLineCallback;

      LineEventsPort( const PortConfig &port );
      virtual ~LineEventsPort();

      virtual bool initialisePort();

      virtual bool openPort();
      virtual bool closePort();

      virtual void ptt( int state );
      virtual void key( int state );

      virtual void checkControls( );
      virtual int getlinesMode()
      {
          return linesMode;
      }
private slots:
      void linesChangedEvent();
};
#endif
