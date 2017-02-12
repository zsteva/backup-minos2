/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2006 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#ifndef ControlPortH
#define ControlPortH 
//==============================================================================
//#include "comport.h"
//#include "portconf.h"
//#include "WindowsMonitor.h"

class commonPort;
class LineConfig;
class PortConfig;
class commonController;

class commonLineControl
{
      bool state;
      bool lastState;
   public:
      QString lineName;
      QString portLineName;
      commonPort *port;
      bool lineIn;
      commonLineControl( QString name, bool lineIn, QString portLine, commonPort *port );
      virtual ~commonLineControl();

      void checkLine();

      void setState( bool state );
      bool getState()
      {
         return state;
      }
};
class commonPort
{
   private:

   public:
      QString pName;
      QString portName;
      int openCount;

      commonController *monitor;

      // We will also need a structure to accept the individual lines for this port
      // Then we will have a common line change routine.
      // checkControls will have to report on all relevant line changes

      QVector<commonLineControl *> lines;
      virtual void addLine( const LineConfig &line );
      commonLineControl *findLine( const QString &name, bool lineIn );

      commonPort( const PortConfig &port );
      virtual ~commonPort();

      virtual bool initialise( const PortConfig &port, commonController &monitor );
      virtual bool initialisePort() = 0;

      virtual bool openPort() = 0;
      virtual bool closePort() = 0;

      void checkControls( void );

      virtual void setLine( commonLineControl * ) = 0;
      virtual bool getLine( commonLineControl * ) = 0;
      virtual void getLineState() = 0;
      void reportLineChange( commonLineControl * );

};


//==============================================================================
#ifdef RUBBISH
// this is a particular implementation, created by factory and not generally seen
class BasicComPort: public COMPort
{
   public:
      virtual void processRxBuff()
      {
         rxBuff.clear();
      }
};

class serialPort: public commonPort
{
   private:
#define COMMSBUFFSIZE 0x200

      int speed;
      int parity;
      int bits;
      int stopbits;

      COMPort::MSPack msr;
      BasicComPort aPort;

   public:

      //      static serialPort *C1;
      //      static serialPort *C2;

      serialPort( const PortConfig &port );
      virtual ~serialPort();

      virtual bool initialisePort();

      /* Set up the port */

      virtual bool openPort();
      virtual bool closePort();

      void setLine( commonLineControl * );
      bool getLine( commonLineControl * );
      void getLineState();
};
#endif
//==============================================================================

// this is a particular implementation, created by factory and not generally seen

class WindowsMonitorPort: public commonPort
{
   private:
    QWidget *parent;
   public:

      WindowsMonitorPort(QWidget *p, const PortConfig &port );
      virtual ~WindowsMonitorPort();

      virtual bool initialisePort();

      virtual bool openPort();
      virtual bool closePort();

      void setLine( commonLineControl * );
      bool getLine( commonLineControl * );
      void getLineState();
};
#ifdef RUBBISH
class K8055Port: public commonPort
{
   private:

      int addr;
      long dig;
   public:
      K8055Port( const PortConfig &port );
      virtual ~K8055Port();

      virtual bool initialisePort();

      virtual bool openPort();
      virtual bool closePort();

      void setLine( commonLineControl * );
      bool getLine( commonLineControl * );
      void getLineState();
};
#endif
#ifdef RUBBISH
class UBWPort: public commonPort
{
   private:

      long dig;
   public:
      UBWPort( const PortConfig &port );
      virtual ~UBWPort();

      virtual bool initialisePort();

      virtual bool openPort();
      virtual bool closePort();

      void setLine( commonLineControl * );
      bool getLine( commonLineControl * );
      void getLineState();
};
#endif
#endif
