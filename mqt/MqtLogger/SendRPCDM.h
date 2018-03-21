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
#include "RigCache.h"
#include "RotatorCache.h"

//---------------------------------------------------------------------------
class MinosRPCObj;
class TSingleLogFrame;
//---------------------------------------------------------------------------
enum RotateDirection : int;
class TSendDM : public QObject
{
    Q_OBJECT
   private:  	// User declarations
      RigCache rigCache;
      RotatorCache rotatorCache;

   public:  		// User declarations
      TSendDM( QWidget* Owner );
      ~TSendDM();

      void sendKeyerPlay( TSingleLogFrame *tslf,int fno );
      void sendKeyerRecord(TSingleLogFrame *tslf, int fno );
      void sendBandMap( TSingleLogFrame *tslf,const QString &freq, const QString &call, const QString &utc, const QString &loc, const QString &qth );
      void sendKeyerTone(TSingleLogFrame *tslf);
      void sendKeyerTwoTone(TSingleLogFrame *tslf);
      void sendKeyerStop(TSingleLogFrame *tslf);

      void sendSelectRotator(TSingleLogFrame *tslf, QString);
      void sendRotator(TSingleLogFrame *tslf,rpcConstants::RotateDirection direction, int angle );

      void sendSelectRig(TSingleLogFrame *tslf,QString);
      void sendRigControlFreq(TSingleLogFrame *tslf,const QString &freq);
      void sendRigControlMode(TSingleLogFrame *tslf, const QString &mode);
      void sendRigControlPassBandState(TSingleLogFrame *tslf,const int state);

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
      void setRadioName(QString);

      void setKeyerLoaded();

};
#endif
