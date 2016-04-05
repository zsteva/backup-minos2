/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////

#ifndef SendRPCDMH
#define SendRPCDMH 
#include <QObject>
#include <QWidget>
#include <QString>
#include <QTimer>

class MinosRPCObj;
//---------------------------------------------------------------------------
class MinosRPCObj;
//---------------------------------------------------------------------------
enum RotateDirection : int;
class TSendDM : public QObject
{
    Q_OBJECT
   private:  	// User declarations
    QTimer ConnectTimer;
    QTimer SubscribeTimer;
      bool connected;
      bool subscribed;
      QString serverName;
      QString keyerServerName;
      QString rigServerName;
      QString bandMapServerName;
      QString rotatorServerName;

      void logMessage( QString s );
      void makeRPCObjects();
      void notifyCallback( bool err, MinosRPCObj *mro, const QString &from );
      void bandMapClientCallback( bool err, MinosRPCObj *mro, const QString &from );
      void bandMapServerCallback( bool err, MinosRPCObj *mro, const QString &from );
      void keyerCallback( bool err, MinosRPCObj *mro, const QString &from );
      void rotatorCallback( bool err, MinosRPCObj *mro, const QString &from );
      void zconfCallback( bool err, MinosRPCObj *mro, const QString &from );
      void loggerServerCallback( bool err, MinosRPCObj *mro, const QString &from );
   public:  		// User declarations
      TSendDM( QWidget* Owner );
      ~TSendDM();
      void subscribeAll();
      static void sendKeyerPlay( int fno );
      static void sendKeyerRecord( int fno );
      static void sendBandMap( const QString &freq, const QString &call, const QString &utc, const QString &loc, const QString &qth );
      static void sendKeyerTone();
      static void sendKeyerTwoTone();
      static void sendKeyerStop();
      static void sendRotator(RotateDirection direction, int angle );
      void doSendKeyerPlay( int fno );
      void doSendKeyerRecord( int fno );
      void doSendBandMap( const QString &freq, const QString &call, const QString &utc, const QString &loc, const QString &qth );
      void doSendKeyerTone();
      void doSendKeyerTwoTone();
      void doSendKeyerStop();
      void doSendRotator(RotateDirection, int angle );
     private slots:
      void ConnectTimerTimer();
      void SubscribeTimerTimer();
};
//---------------------------------------------------------------------------
extern TSendDM *SendDM;
//---------------------------------------------------------------------------
#endif
