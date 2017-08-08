/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#ifndef keyersH
#define keyersH
#include "base_pch.h"

#include "keyconf.h"
#include "keyctrl.h"
#include "keyerlog.h"

#include <deque>

enum LINEMODES{
    elmNone = 0,
    elmRecord,
    elmPlayPip,
    elmPlayNoPip,
    elmTestPlay,
    elmTones,   //5
    elm6,
    elm7,
    elm8,
    elm9,
    elm10,
    elm11,
    elm12,
    elmMGM,                 // 13
    elmAppsRestartClose,    // 14
    elmOSRestartClose,      // 15
    elmMax                  // 16
};

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

extern unsigned long MORSEINTCOUNT;
extern qint64 currTick;
//extern qint64 basetick;

class SoundSystemDriver;

class commonPort;
class commonKeyer;

class RecBuffer
{
   public:
      int RecBlock;
      int WriteBlock;

      char buff[ 40000 ]; // should be same size as sound buffers
      int Size;
      bool filled;
      RecBuffer() : Size( 0 ), filled( false ), RecBlock( -1 ), WriteBlock( -1 )
      {}
      void reset()
      {
         RecBlock = -1;
         WriteBlock = -1;
         Size = 0;
         filled = false;
      }
};

class lineMonitor
{
   private:
      commonPort *cp;
   public:
      PortConfig pconf;
      KeyerConfig kconf;
      QString pName;
      lineMonitor( const QString pname );
      lineMonitor( const KeyerConfig &keyer, const PortConfig &port );
      virtual ~lineMonitor();

      virtual void ptt( int state );
      virtual void key( int state );

      virtual bool pttChanged( int state ) = 0;
      virtual bool L1Changed( int state ) = 0;
      virtual bool L2Changed( int state ) = 0;
      virtual bool linesModeChanged(int lmode) = 0;

      virtual bool initialise( const KeyerConfig &keyer, const PortConfig &port ) = 0;

      virtual void checkControls( void );
};
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
// all keyers want timer ticks to sequence the state machine
class commonKeyer: public lineMonitor, public timerTicker
{
   public:
      commonKeyer( const KeyerConfig &keyer, const PortConfig &port );
      ~commonKeyer();

      virtual bool pttChanged( int state );
      virtual bool L1Changed( int state );
      virtual bool L2Changed( int state );
      virtual bool linesModeChanged(int lmode);

      virtual void tickEvent();       // this will often be an interrupt routine
      virtual bool getInfo( KeyerInfo * ) = 0;

      virtual bool initialise( const KeyerConfig &keyer, const PortConfig &port ) = 0;
      virtual void select( bool ) = 0;

      virtual bool docommand( const KeyerCtrl &dvp_ctrl ) = 0;

      virtual bool getPTT( void );
      virtual void getActionState( QString & );
      virtual bool getStatus( QString &a );
      virtual bool startMicPassThrough() = 0;
      virtual bool stopMicPassThrough() = 0;
      virtual bool sendCW( const char *message, int speed, int tone ) = 0;

      virtual void initTone1( int ) = 0;
      virtual void initTone2( int, int ) = 0;
      virtual void startTone1() = 0;
      virtual void startTone2() = 0;
      virtual void queueFinished();

      bool started;
      int startcount;

      bool L1State;
      bool L2State;
      bool pttState;
      int linesMode;
      virtual void enableQueue( bool /*b*/ )
      {}
      bool boxRecPending;
      bool recPending;

      double cwRate;
      unsigned long lastIntCount;

      int tone1;
      int tone2;

      bool inTone;
};

class sbKeyer
{
   public:
      sbKeyer();
      virtual ~sbKeyer();
      void sbTickEvent();       // this will often be an interrupt routine
      bool sbInitialise(int rate, int pipTone, int pipVolume, int pipLength , int filterCorner);
      void sbInitTone1( int );
      void sbInitTone2( int, int );
      void sbStartTone1();
      void sbStartTone2();
};

class voiceKeyer: public commonKeyer, public sbKeyer
{
   public:

      voiceKeyer( const KeyerConfig &keyer, const PortConfig &port );
      ~voiceKeyer();
      bool docommand( const KeyerCtrl &dvp_ctrl );

      virtual bool getInfo( KeyerInfo * );
      virtual bool pttChanged( int state );
      virtual bool L1Changed( int state );
      virtual bool L2Changed( int state );
      virtual bool linesModeChanged(int lmode);
      virtual bool initialise( const KeyerConfig &keyer, const PortConfig &port );
      virtual void select( bool );
      virtual bool sendCW( const char *message, int speed, int tone );
      virtual bool startMicPassThrough();
      virtual bool stopMicPassThrough();
      void tickEvent();       // this will often be an interrupt routine
      virtual void initTone1( int );
      virtual void initTone2( int, int );
      virtual void startTone1();
      virtual void startTone2();
};

//=============================================================================

extern int tuneTime;
extern double tuneLevel;
extern int CWTone;
extern int CWSpeed;

extern commonKeyer *currentKeyer;

extern QMap <char, QString> MORSECODE;    // . is 0x40, - is 0x80
class MORSEMSG
{
   public:
      MORSEMSG();
      ~MORSEMSG();
      void setMessage( const QString &msg );
      QString msg;
      bool repeat;
};
extern QMap <int, MORSEMSG> MORSEMSGS;


//=============================================================================
class KeyerAction
{
   protected:
//      qint64 startTick;
      qint64 lastTick;
   public:
      int pipStartDelaySamples;
      bool tailWithPip;

      long actionTime;
      bool deleteAtTick;
      void checkTimer();
      virtual void getActionState( QString &s ) = 0;

      virtual void LxChanged( int line, bool state ) = 0;
      virtual void pttChanged( bool state ) = 0;
      virtual void linesModeChanged(int lmode) = 0;
      virtual void queueFinished() = 0;
      virtual void timeOut() = 0;
      virtual void stopTransmit();

      KeyerAction();
      virtual ~KeyerAction();
      static my_deque < KeyerAction *> currentAction;
      static KeyerAction *getCurrentAction();
      KeyerAction *getNextAction();
      virtual char statusLetter() = 0;
      virtual bool playingFile( const QString & );
      virtual void activateVox( void );
      virtual void interruptOK( void ) = 0;

      virtual RecBuffer *getSourceBuffer(){return 0;}
      virtual void doSinkBuffer(RecBuffer *){}

      long getActionTime()
      {
         return actionTime;
      }
};
//=============================================================================
class ToneAction: public KeyerAction
{
      enum ToneActionStates {etasInitial = -1, etasPTTDelay, etasStartTone, etasStopTone} actionState;
      int nTone;
   public:
      long delayTime;

      virtual void getActionState( QString &s );
      virtual void LxChanged( int line, bool state );
      virtual void pttChanged( bool state );
      virtual void linesModeChanged(int lmode);
      virtual void queueFinished();
      virtual void timeOut();
      virtual void interruptOK( void );
      char statusLetter()
      {
         return 't';
      }
      ToneAction( int tones, long pdelayStart );
      virtual ~ToneAction();
};
//=============================================================================
class VoiceAction: public KeyerAction
{
   public:
      virtual void getActionState( QString &s ) = 0;
      virtual void LxChanged( int line, bool state ) = 0;
      virtual void pttChanged( bool state ) = 0;
      virtual void linesModeChanged(int lmode) = 0;
      virtual void queueFinished() = 0;
      virtual void timeOut() = 0;
      virtual void interruptOK( void );
      VoiceAction();
      virtual ~VoiceAction();
};
//=============================================================================
class InitialPTTAction: public VoiceAction
{
      enum initPTTActionStates {einitPTTInitial = -1, einitPTTStart, einitPTTEnd, einitPTTFlickRelease, einitPTTRelease} actionState;
   public:
      virtual void getActionState( QString &s );
      virtual void LxChanged( int line, bool state );
      virtual void pttChanged( bool state );
      virtual void linesModeChanged(int lmode);
      virtual void queueFinished();
      virtual void timeOut();
      InitialPTTAction();
      virtual ~InitialPTTAction();
      char statusLetter()
      {
         return 'T';
      }
};
//=============================================================================
class InterruptingPTTAction: public VoiceAction
{
      enum InterruptPTTActionStates {einterPTTInitial = -1, einterPTTWaitDelay,
                                     einterPTTWaitDelayFinish, einterPTTDoPip, einterPTTQuickRelease} actionState;
   public:
      virtual void getActionState( QString &s );
      virtual void LxChanged( int line, bool state );
      virtual void pttChanged( bool state );
      virtual void linesModeChanged(int lmode);
      virtual void queueFinished();
      virtual void timeOut();
      InterruptingPTTAction();
      virtual ~InterruptingPTTAction();
      char statusLetter()
      {
         return 'T';
      }
};
//=============================================================================
class PlayAction: public VoiceAction
{
      QString fileName;
      long delayTime;
      long repeatTime;
      bool testMode;
      bool CW;
      enum PlayActionStates {epasInitial = -1, epasWaitRepeat, epasWaitLinear, epasPlayFile,
                             epasEndPlayFile} actionState;
      QString ActionStateString;
   public:
      virtual void getActionState( QString &s );
      virtual void LxChanged( int line, bool state );
      virtual void pttChanged( bool state );
      virtual void linesModeChanged(int lmode);
      virtual void queueFinished();
      virtual void timeOut();
      PlayAction( const QString &fileName, bool noPTT, long delayStart, long repeatDelay, bool firstTime, bool CW );
      virtual ~PlayAction();
      char statusLetter()
      {
         return 'S';
      }
      virtual bool playingFile( const QString & );
};
//=============================================================================
class PipAction: public VoiceAction
{
      enum PipActionStates {epipasInitial = -1, epipasPip, epipasEndPip} actionState;
   public:
      virtual void getActionState( QString &s );
      virtual void LxChanged( int line, bool state );
      virtual void pttChanged( bool state );
      virtual void linesModeChanged(int lmode);
      virtual void queueFinished();
      virtual void timeOut();
      PipAction();
      virtual ~PipAction();
      char statusLetter()
      {
         return 'P';
      }
};
//=============================================================================
class RecordAction: public VoiceAction
{
      QString fileName;
      enum RecordActionStates {erasInitial = -1, erasStartRec, erasStopRec, erasRecFinished, erasRecWaitPTT} actionState;
   public:
      virtual void getActionState( QString &s );
      virtual void LxChanged( int line, bool state );
      virtual void pttChanged( bool state );
      virtual void linesModeChanged(int lmode);
      virtual void queueFinished();
      virtual void timeOut();
      RecordAction( const QString &fileName );
      virtual ~RecordAction();
      char statusLetter()
      {
         return 'R';
      }
};
//=============================================================================
class BoxRecordAction: public VoiceAction
{
      enum BoxRecordActionStates {ebrasInitial = -1} actionState;
   public:
      virtual void getActionState( QString &s );
      virtual void LxChanged( int line, bool state );
      virtual void pttChanged( bool state );
      virtual void linesModeChanged(int lmode);
      virtual void queueFinished();
      virtual void timeOut();
      BoxRecordAction();
      virtual ~BoxRecordAction();
      char statusLetter()
      {
         return 'B';
      }
};
//=============================================================================
#endif
