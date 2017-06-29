/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////

#ifndef ConfigDMH
#define ConfigDMH 

#include <QWidget>
#include <QProcess>
//---------------------------------------------------------------------------
extern bool terminated;
class TConfigElement: public QObject
{
    Q_OBJECT
   private:  	// User declarations
    QProcess *runner;
   public:  		// User declarations
      QString name;
      QString outfname;
      //   QString progname;
      QString rundir;
      QString commandLine;
      QString params;

      bool run;

      TConfigElement();
      bool initialise( QString sect );

      void setRun(bool state);

      void createProcess();
private slots:
      void	on_started();
      void on_finished(int, QProcess::ExitStatus exitStatus);
      void	on_error(QProcess::ProcessError error);

      void	on_readyReadStandardError();
      void	on_readyReadStandardOutput();
      //void	on_stateChanged(QProcess::ProcessState newState);
};
class TMConfigDM : public QObject
{
    Q_OBJECT
   private:  	// User declarations
      static TMConfigDM *thisDM;
      TMConfigDM( QWidget* Owner );

      QVector <QProcess *> guardv;
      QString circleOfHell;
      bool hideServers;
      bool autoStart;
   public:  		// User declarations
      ~TMConfigDM();

      QVector <QSharedPointer<TConfigElement> > elelist;

      void start();
      void stop();
      void setCircleOfHell( const QString &circle );
      QString getCircleOfHell();

      bool getAutoStart();
      void setAutoStart(bool);
      bool getHideServers();
      void setHideServers(bool);
      static TMConfigDM *getConfigDM( QWidget* Owner );
};
//---------------------------------------------------------------------------
#endif
