/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#ifndef CommonMonitorH
#define CommonMonitorH 
//---------------------------------------------------------------------------
#include "base_pch.h"

class commonController;
class commonPort;
class commonLineControl;
class LineConfig;
class PortConfig;

//#define TARGET_RESOLUTION 55U         // 55-millisecond target resolution
//#define TIMER_INTERVAL 55U         // 55-millisecond target interval

void setLines( bool PTTOut, bool PTTIn, bool L1, bool L2 );

class timerTicker: public QObject
{
    Q_OBJECT
      QTimer b;
   public:
      timerTicker();
      ~timerTicker();
      virtual void tickEvent() = 0;       // this will often be an interrupt routine

private slots:
      void tick()
      {
          tickEvent();
      }
};

class commonController: public timerTicker
{
    QWidget *parent;
   public:
      commonController(QWidget *p);
      ~commonController();
      my_deque < commonPort *> portChain;
      commonPort *createPort( const PortConfig &port );
      commonLineControl *findLine(const QString &name, bool lineIn );

      bool initialise( );
      void closeDown();

      virtual void tickEvent();       // this will often be an interrupt routine
      void checkControls( void );

      //void ptt(int);
      //void key(int);
      void lineChange( commonLineControl *line );
};
#endif
