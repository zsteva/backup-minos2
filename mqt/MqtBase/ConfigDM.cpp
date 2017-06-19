/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------

#include "base_pch.h"

#include "ConfigDM.h"
#include "ConfigFile.h"
//---------------------------------------------------------------------------

bool terminated = false;
bool configExists = false;

/*static*/
TMConfigDM *TMConfigDM::thisDM = 0;
/*static*/
TMConfigDM *TMConfigDM::getConfigDM(QWidget *Owner )
{
   if ( !thisDM )
   {
      thisDM = new TMConfigDM( Owner );
   }
   return thisDM;
}
//---------------------------------------------------------------------------
TConfigElement::TConfigElement()
      : run( false ), runner(0)
{}
//---------------------------------------------------------------------------
bool TConfigElement::initialise( QString sect )
{
    if (!configExists)
        return false;

    QSettings config(ConfigFile::getConfigIniName(), QSettings::IniFormat);

    name = sect;
    QString noprog = "nOpRoGrAm";

    commandLine = config.value( sect + "/program", noprog ).toString().trimmed();
    params = config.value( sect + "/params", "" ).toString();
    if ( commandLine == noprog )
    {
        commandLine.clear();
        QString server = config.value( sect + "/Server", "localhost" ).toString().trimmed();
        if (server.compare("localhost") == 0)
            return false;
    }
    QString S = config.value( sect + "/Run", "No" ) .toString().trimmed();
    run = ( S[ 0 ] == 'R' || S[ 0 ] == 'Y' );

    rundir = config.value( sect + "/Directory", "" ).toString().trimmed();
    return true;
}
void TConfigElement::setRun(bool state)
{
    QSettings config(ConfigFile::getConfigIniName(), QSettings::IniFormat);
   config.setValue(name + "/Run", state?"Yes":"No");
   //and if we are started, modify this elements state
}
void TConfigElement::createProcess()
{
    if ( run && !runner)
    {
        runner = new QProcess(parent());
/*
        QString program = commandLine;
        if (!FileExists(program))
        {
            trace(name + ":program doesn't exist:" + program);
        }
            runner->setProgram(program);

        QStringList args;
        args.append(params);
        runner->setArguments(args);
*/
        QString wdir = rundir;
        runner->setWorkingDirectory(wdir);

        connect (runner, SIGNAL(started()), this, SLOT(on_started()));
        connect (runner, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_finished(int, QProcess::ExitStatus)));
        connect (runner, SIGNAL(error(QProcess::ProcessError)), this, SLOT(on_error(QProcess::ProcessError)));

        connect (runner, SIGNAL(readyReadStandardError()), this, SLOT(on_readyReadStandardError()));
        connect (runner, SIGNAL(readyReadStandardOutput()), this, SLOT(on_readyReadStandardOutput()));

        runner->start(commandLine);
    }
}

void TConfigElement::on_started()
{
    trace(name + ":started");
}

void TConfigElement::on_finished(int err, QProcess::ExitStatus exitStatus)
{
    trace(name + ":finished:" + QString::number(err) + ":" + QString::number(exitStatus));
    runner->start();    // but we have to be careul when we close!
}

void TConfigElement::on_error(QProcess::ProcessError error)
{
    trace(name + ":error:" + QString::number(error));
}

void TConfigElement::on_readyReadStandardError()
{
    QString r = runner->readAllStandardError();
    trace(name + ":stdErr:" + r);
}

void TConfigElement::on_readyReadStandardOutput()
{
    QString r = runner->readAllStandardOutput();
    trace(name + ":stdOut:" + r);
}

//---------------------------------------------------------------------------
TMConfigDM::TMConfigDM( QWidget* Owner )
      : QObject( Owner )
{
   if ( FileExists( ConfigFile::getConfigIniName() ) )
   {
       configExists = true;
       QSettings config(ConfigFile::getConfigIniName(), QSettings::IniFormat);
      QStringList lsect = config.childGroups();

      for ( int i = 0; i < lsect.count(); i++ )
      {
         QString sect = lsect[ i ].trimmed();
         if ( sect.compare("CIRCLEOFHELL", Qt::CaseInsensitive ) == 0)
         {
            getCircleOfHell();
         }
         if ( sect.compare("Settings", Qt::CaseInsensitive ) == 0)
         {
            getAutoStart();
            getHideServers();
         }
         else
         {
            QSharedPointer<TConfigElement> tce = QSharedPointer<TConfigElement>(new TConfigElement());
            if ( tce->initialise( sect ) )
            {
               elelist.push_back( tce );
            }
         }
      }
   }
}
//---------------------------------------------------------------------------
TMConfigDM::~TMConfigDM()
{
   if ( !terminated )
      stop();

   elelist.clear();
}
//---------------------------------------------------------------------------
void TMConfigDM::start()
{
   if ( !configExists )
   {
      return ;
   }
   terminated = false;
//   resetCloseEvent();

   for ( QVector <QSharedPointer<TConfigElement> >::iterator i = elelist.begin(); i != elelist.end(); i++ )
   {
       (*i)->createProcess();
   }
}

void TMConfigDM::stop()
{
   if ( !configExists )
   {
      return ;
   }
   terminated = true;
//   signalCloseEvent();

   for ( QVector <QProcess *> ::iterator i = guardv.begin(); i != guardv.end(); i++ )
   {
      if ( ( *i ) )
      {
         logMessage( "Killing subProcess", "" );
         ( *i ) ->terminate();
         logMessage( "subProcess killed", "" );
         delete ( *i );
      }
   }
   logMessage( "All guardian threads finished", "" );
   guardv.clear();

}
void TMConfigDM::setCircleOfHell( const QString &circle )
{
   if ( !configExists )
   {
      return ;
   }
   circleOfHell = circle;
   QSettings config(ConfigFile::getConfigIniName(), QSettings::IniFormat);
   config.setValue("CircleOfHell/Name", circleOfHell);
}
QString TMConfigDM::getCircleOfHell()
{
   if ( !configExists )
   {
      return "";
   }
   QSettings config(ConfigFile::getConfigIniName(), QSettings::IniFormat);
   circleOfHell = config.value( "CircleOfHell/Name", "" ).toString().trimmed();
   return circleOfHell;
}
bool TMConfigDM::getAutoStart()
{
   if ( !configExists )
   {
      return false;
   }
   QSettings config(ConfigFile::getConfigIniName(), QSettings::IniFormat);
   autoStart = config.value( "Settings/Autostart", false ).toBool();
   return autoStart;
}
void TMConfigDM::setAutoStart(bool s)
{
   if ( !configExists )
   {
      return;
   }
   QSettings config(ConfigFile::getConfigIniName(), QSettings::IniFormat);
   config.setValue( "Settings/Autostart", s );
}
bool TMConfigDM::getHideServers()
{
   if ( !configExists )
   {
      return false;
   }
   QSettings config(ConfigFile::getConfigIniName(), QSettings::IniFormat);
   hideServers = config.value( "Settings/HideServers", false ).toBool();
   return hideServers;
}
void TMConfigDM::setHideServers(bool s)
{
   if ( !configExists )
   {
      return;
   }
   QSettings config(ConfigFile::getConfigIniName(), QSettings::IniFormat);
   config.setValue( "Settings/HideServers", s );
}

