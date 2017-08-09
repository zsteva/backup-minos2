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
      void resetConnectables(LoggerContestLog *ct);

      void sendKeyerPlay( int fno );
      void sendKeyerRecord( int fno );
      void sendBandMap( const QString &freq, const QString &call, const QString &utc, const QString &loc, const QString &qth );
      void sendKeyerTone();
      void sendKeyerTwoTone();
      void sendKeyerStop();
      void sendRotator(rpcConstants::RotateDirection direction, int angle );
   private slots:
      void on_serverCall( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );
      void on_clientCall( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );
      void on_notify( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );
signals:
      void RotatorLoaded();
      void setBandMapLoaded();

      void RotatorState(QString);
      void RotatorBearing(QString);
      void RotatorMaxAzimuth(QString);
      void RotatorMinAzimuth(QString);
      void RotatorAntennaName(QString);


      void setMode(QString);
      void setFreq(QString);

      void setKeyerLoaded();

};
#endif
