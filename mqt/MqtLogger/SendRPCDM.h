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

      void sendSelectRotator(QString);
      void sendRotator(rpcConstants::RotateDirection direction, int angle );

      void sendSelectRig(QString);
      void sendRigControlFreq(const QString &freq);
      void sendRigControlMode(const QString &mode);
      void sendRigControlPassBandState(const int state);
   private slots:
      void on_serverCall( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );
      void on_notify( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );

signals:
      void setBandMapLoaded();

      void RotatorLoaded();
      void RotatorList(QString);
      void RotatorState(QString);
      void RotatorBearing(QString);
      void RotatorMaxAzimuth(QString);
      void RotatorMinAzimuth(QString);

      void setRadioLoaded();
      void setMode(QString);
      void setFreq(QString);
      void setRadioState(QString);
      void setRadioTxVertStatus(QString);
      void setRadioList(QString);

      void setKeyerLoaded();

};
#endif
