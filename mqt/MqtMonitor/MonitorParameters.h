#ifndef MONITORPARAMETERS_H
#define MONITORPARAMETERS_H
#include "base_pch.h"

class MonitorParameters : public MinosParametersAdapter
{
      int p1;
      int p2;
   public:
      MonitorParameters() : p1( 10 ), p2( 60 )
      {}
      ~MonitorParameters()
      {}
      virtual int getStatsPeriod1();
      virtual int getStatsPeriod2();
      virtual void setStatsPeriod1( int );
      virtual void setStatsPeriod2( int );
      virtual bool yesNoMessage( QWidget* Owner, const QString &mess ) override;
      virtual void mshowMessage(const QString &mess, QWidget* Owner = 0 ) override;
};

#endif // MONITORPARAMETERS_H
