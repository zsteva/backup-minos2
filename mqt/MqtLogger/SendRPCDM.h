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
#include "logger_pch.h"

#include <QObject>
#include <QWidget>
#include <QString>
#include <QTimer>

#include "ConfigFile.h"

class MinosRPCObj;
//---------------------------------------------------------------------------
class MinosRPCObj;
//---------------------------------------------------------------------------
enum RotateDirection : int;
class TSendDM : public QObject
{
    Q_OBJECT
   private:  	// User declarations

      void logMessage( QString s );

      Connectable keyerServerConnectable;
      Connectable rigServerConnectable;
      Connectable bandMapServerConnectable;
      Connectable rotatorServerConnectable;

   public:  		// User declarations
      TSendDM( QWidget* Owner, LoggerContestLog *ct );
      ~TSendDM();
      void sendKeyerPlay( int fno );
      void sendKeyerRecord( int fno );
      void sendBandMap( const QString &freq, const QString &call, const QString &utc, const QString &loc, const QString &qth );
      void sendKeyerTone();
      void sendKeyerTwoTone();
      void sendKeyerStop();
      void sendRotator(rpcConstants::RotateDirection direction, int angle );
      void doSendKeyerPlay( int fno );
      void doSendKeyerRecord( int fno );
      void doSendBandMap( const QString &freq, const QString &call, const QString &utc, const QString &loc, const QString &qth );
      void doSendKeyerTone();
      void doSendKeyerTwoTone();
      void doSendKeyerStop();
      void doSendRotator(rpcConstants::RotateDirection, int angle );
     private slots:
      void on_request( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );
      void on_response( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );
      void on_notify( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );
};
#endif
