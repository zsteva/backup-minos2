/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------

#include "logger_pch.h"

#include "ConfigDM.h"
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

    QSettings config("./Configuration/MinosConfig.ini", QSettings::IniFormat);

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
    QSettings config("./Configuration/MinosConfig.ini", QSettings::IniFormat);
   config.setValue(name + "/Run", state?"Yes":"No");
   //and if we are started, modify this elements state
}
void TConfigElement::createProcess()
{
    if ( run && !runner)
    {
        runner = new QProcess(parent());

        QString program = commandLine;
        runner->setProgram(program);

        //QStringList args;
        //runner->setArguments(args);

        QString wdir = rundir;
        runner->setWorkingDirectory(wdir);

        connect (runner, SIGNAL(started()), this, SLOT(on_started()));
        connect (runner, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_finished(int, QProcess::ExitStatus)));
        connect (runner, SIGNAL(error(QProcess::ProcessError)), this, SLOT(on_error(QProcess::ProcessError)));

        connect (runner, SIGNAL(readyReadStandardError()), this, SLOT(on_readyReadStandardError()));
        connect (runner, SIGNAL(readyReadStandardOutput()), this, SLOT(on_readyReadStandardOutput()));

        runner->start();
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
    trace(name + "error:" + QString::number(error));
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
   if ( FileExists( "./Configuration/MinosConfig.ini" ) )
   {
       configExists = true;
       QSettings config("./Configuration/MinosConfig.ini", QSettings::IniFormat);
      QStringList lsect = config.childGroups();

      for ( int i = 0; i < lsect.count(); i++ )
      {
         QString sect = lsect[ i ].trimmed();
         if ( sect.compare("CIRCLE OF HELL", Qt::CaseInsensitive ) == 0)
         {
            getCircleOfHell();
         }
         else
         {
            TConfigElement *tce = new TConfigElement();
            if ( tce->initialise( sect ) )
            {
               elelist.push_back( tce );
            }
            else
               delete tce;
         }
      }
   }
}
//---------------------------------------------------------------------------
TMConfigDM::~TMConfigDM()
{
   if ( !terminated )
      stop();

   for ( std::vector <TConfigElement *>::iterator i = elelist.begin(); i != elelist.end(); i++ )
   {
      delete ( *i );
   }
   elelist.clear();
}
//---------------------------------------------------------------------------
void GuardianThreadExecute( void *arg )
{
    /*
   TConfigElement * s = ( TConfigElement * ) arg;
   QString app = s->name;
   TGuardianThread *guardth = new TGuardianThread( s );
   while ( !terminated )
   {
      guardth->execute();  // one pass at a time...
      if ( terminated )
         break;
      Sleep( 1000 );
   }
   delete guardth;
   */
}
//---------------------------------------------------------------------------
void TMConfigDM::start()
{
   if ( !configExists )
   {
      return ;
   }
   terminated = false;
   resetCloseEvent();

   for ( std::vector <TConfigElement *>::iterator i = elelist.begin(); i != elelist.end(); i++ )
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
   signalCloseEvent();

   for ( std::vector <QProcess *> ::iterator i = guardv.begin(); i != guardv.end(); i++ )
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
   QSettings config("./Configuration/MinosConfig.ini", QSettings::IniFormat);
   config.setValue("Circle Of Hell/Name", circleOfHell);
}
QString TMConfigDM::getCircleOfHell()
{
   if ( !configExists )
   {
      return "";
   }
   QSettings config("./Configuration/MinosConfig.ini", QSettings::IniFormat);
   circleOfHell = config.value( "Circle Of Hell/Name", "" ).toString().trimmed();
   return circleOfHell;
}

#ifdef RUBBISH

// How to find if parent has disappeared - this needs to go in the children
// So we need a "starter" process for where we don't want the logger (e.g. voice keyer)

#ifndef PROCESS_H
#define PROCESS_H

class Process
{
public:

static unsigned long currentPID();
static unsigned long parentPID();

static bool isParentRunning();

};

#endif // PROCESS_H
@

process.cpp
@

#include "process.h"

#ifdef WIN32
#include <windows.h>
#include <tlhelp32.h>
#else
#include "unistd.h"
#endif

unsigned long Process::currentPID()
{
#ifdef WIN32
return GetCurrentProcessId();
#else
return getpid();
#endif
}

unsigned long Process::parentPID()
{
#ifdef WIN32

HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
PROCESSENTRY32 pe = { 0 };
pe.dwSize = sizeof(PROCESSENTRY32);

unsigned long pid = currentPID();
unsigned long ppid = 0;

if( Process32First(h, &pe)) {
do {
if (pe.th32ProcessID == pid) {
ppid = pe.th32ParentProcessID;
break;
}
} while( Process32Next(h, &pe));
}

CloseHandle(h);

return ppid;

#else

return getppid();

#endif
}

bool Process::isParentRunning()
{
#ifdef WIN32

static unsigned long _parentPID = parentPID();
static void* _parentHandle = NULL;

if (_parentHandle == NULL && _parentPID != 0)
_parentHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, _parentPID);

if (_parentHandle != NULL)
{
BOOL success;
DWORD exitCode;

success = GetExitCodeProcess(_parentHandle, &exitCode);

return ( ! success) || exitCode == STILL_ACTIVE;
}

return true;

#else
return getppid() != 1;
#endif
}
#endif
