#include "mqtUtils_pch.h"

#include <QSettings>
#include <QHostInfo>
#include "fileutils.h"
#include "ConfigFile.h"

//---------------------------------------------------------------------------

bool terminated = false;
bool configExists = false;
static QStringList runTypeList{"None", "RunLocal", "ConnectLocal", "ConnectServer"};
static QStringList appTypeList{
    "None",
    "AppStarter",
    "BandMap",
    "Chat",
    "Keyer",
    "LineControl",
    "Logger",
    "Monitor",
    "Other",
    "RigControl",
    "Rotator",
    "RotatorControl",
    "Server"
};
static QStringList defaultAppList{
    "",
    "./mqtAppStarter",
    "./mqtBandMap",
    "./mqtChat",
    "./mqtKeyer",
    "./mqtControl",
    "./mqtLogger",
    "./mqtMonitor",
    "",
    "./mqtRigControl",
    "./mqtRotator",
    "./mqtRotatorControl",
    "./mqtServer"
};

QString getRunType(RunType r)
{
    QString res;
    if (r < rtMax)
        res = runTypeList[r];
    return res;
}
QString getAppType(AppType r)
{
    QString res;
    if (r < atMax)
        res = appTypeList[r];
    return res;
}
QString getDefaultApp(AppType r)
{
    QString res;
    if (r < atMax)
        res = defaultAppList[r];
#ifdef Q_OS_WIN
    if (!res.isEmpty())
        res += ".exe";
#endif
    return res;
}

/*static*/
MinosConfig *MinosConfig::thisDM = 0;
/*static*/
MinosConfig *MinosConfig::getMinosConfig(QWidget *Owner )
{
   if ( !thisDM )
   {
      thisDM = new MinosConfig( Owner );
   }
   return thisDM;
}
/*static*/
QString MinosConfig::getConfigIniName()
{
    return "./Configuration/MinosConfig.ini";
}
/*static*/
bool MinosConfig::doesConfigExist()
{
    return FileExists( getConfigIniName());
}
/*static*/
QString MinosConfig::getThisServerName()
{
    QSettings config(getConfigIniName(), QSettings::IniFormat);
    QString serverName = config.value( "Settings/ServerName", QHostInfo::localHostName() ).toString().trimmed();

    if ( serverName.size() == 0 )
    {
        QString h = QHostInfo::localHostName();
        serverName = h;
    }
    return serverName;
}
/*static*/
QStringList MinosConfig::getAppTypes()
{
    return appTypeList;
}

//---------------------------------------------------------------------------
TConfigElement::TConfigElement()
      : runType( rtNone ), appType(atOther), runner(0), stopping(false)
{}
//---------------------------------------------------------------------------
bool TConfigElement::initialise( QSettings &config, QString sect )
{
    if (!configExists)
        return false;

    name = sect;

    commandLine = config.value( sect + "/Program", "" ).toString().trimmed();
    server = config.value( sect + "/Server", "localhost" ).toString().trimmed();
    params = config.value( sect + "/Params", "" ).toString().trimmed();
    rundir = config.value( sect + "/Directory", "" ).toString().trimmed();
    remoteApp = config.value(sect + "/RemoteApp", name).toString().trimmed();

    QString S = config.value( sect + "/RunType", getRunType(rtNone) ) .toString().trimmed();

    runType = rtNone;
    for (int i = 0; i < rtMax; i++)
    {
        RunType rt = static_cast<RunType>(i);
        if (S.compare(getRunType(rt), Qt::CaseInsensitive) == 0)
        {
            runType = rt;
            break;
        }

    }

    S = config.value( sect + "/AppType", getAppType(atOther) ) .toString().trimmed();

    appType = atOther;
    for (int i = 0; i < atMax; i++)
    {
        AppType at = static_cast<AppType>(i);
        if (S.compare(getAppType(at), Qt::CaseInsensitive) == 0)
        {
            appType = at;
            break;
        }
    }
    return true;
}
void TConfigElement::save(QSettings &config)
{
    if (name.isEmpty())
    {
        name = getAppType(appType);
    }
    if (name.compare("<Deleted>", Qt::CaseInsensitive) != 0)
    {
        config.setValue(name + "/Program", commandLine);
        config.setValue(name + "/Params", params);
        config.setValue(name + "/Directory", rundir);
        config.setValue(name + "/Server", server);
        config.setValue(name + "/RemoteApp", remoteApp);
        config.setValue(name + "/RunType", getRunType(runType));
        config.setValue(name + "/AppType", getAppType(appType));
    }
}
Connectable TConfigElement::connectable()
{
    Connectable res;
    res.appName = name;
    res.appType = appType;
    res.runType = runType;
    if (runType == rtConnectServer)
    {
        res.serverName = server;
        res.remoteAppName = remoteApp;
    }
    else
    {
        res.serverName = MinosConfig::getThisServerName();
        res.remoteAppName = name;
    }
    return res;
}

void TConfigElement::createProcess()
{
    if ( runType == rtRunLocal && !runner)
    {
        runner = new QProcess(parent());

        QString program = commandLine;
        if (!FileExists(program))
        {
            trace(name + ":program doesn't exist:" + program);
        }

        commandLine += " ";
        commandLine += params;

        QString wdir = rundir;
        runner->setWorkingDirectory(wdir);

        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("MQTRPCNAME", name); // Add an environment variable for the RPC name to use
        runner->setProcessEnvironment(env);

        connect (runner, SIGNAL(started()), this, SLOT(on_started()));
        connect (runner, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_finished(int, QProcess::ExitStatus)));
        connect (runner, SIGNAL(error(QProcess::ProcessError)), this, SLOT(on_error(QProcess::ProcessError)));

        connect (runner, SIGNAL(readyReadStandardError()), this, SLOT(on_readyReadStandardError()));
        connect (runner, SIGNAL(readyReadStandardOutput()), this, SLOT(on_readyReadStandardOutput()));

        runner->start(commandLine);

    }
}
void TConfigElement::stopProcess()
{
    if (runner)
    {
        stopping = true;
        runner->terminate();
    }
}

void TConfigElement::sendCommand(const QString & cmd)
{
    if (runner)
    {
        QByteArray command = (cmd + "\n").toUtf8();
        runner->write( command );
    }
}

void TConfigElement::on_started()
{
    trace(name + ":started");
}

void TConfigElement::on_finished(int err, QProcess::ExitStatus exitStatus)
{
    trace(name + ":finished:" + QString::number(err) + ":" + QString::number(exitStatus));
    if (runner)
    {

        if (stopping)
        {
            stopping = false;
            runner->deleteLater();
            runner = 0;

        }
        else
        {
            runner->start();    // but we have to be careful when we close!
        }
    }
}

void TConfigElement::on_error(QProcess::ProcessError error)
{
    trace(name + ":error:" + QString::number(error));
}

void TConfigElement::on_readyReadStandardError()
{
    if (runner)
    {
        QString r = runner->readAllStandardError();
        trace(name + ":stdErr:" + r);
    }
}

void TConfigElement::on_readyReadStandardOutput()
{
    if (runner)
    {
        QString r = runner->readAllStandardOutput();
        trace(name + ":stdOut:" + r);
    }
}

//---------------------------------------------------------------------------
MinosConfig::MinosConfig( QWidget* Owner )
      : QObject( Owner )
{
   if ( FileExists( getConfigIniName() ) )
   {
       configExists = true;
       QSettings config(getConfigIniName(), QSettings::IniFormat);
      QStringList lsect = config.childGroups();

      for ( int i = 0; i < lsect.count(); i++ )
      {
         QString sect = lsect[ i ].trimmed();
         if ( sect.compare("Settings", Qt::CaseInsensitive ) == 0)
         {
             thisServerName = config.value( "Settings/ServerName", "" ).toString().trimmed();

             if ( thisServerName.size() == 0 )
             {
                 QString h = QHostInfo::localHostName();
                 thisServerName = h;
             }
            autoStart = config.value( "Settings/AutoStart", false ).toBool();
            hideServers = config.value( "Settings/HideServers", false ).toBool();
         }
         else
         {
            QSharedPointer<TConfigElement> tce = QSharedPointer<TConfigElement>(new TConfigElement());
            if ( tce->initialise( config, sect ) )
            {
               elelist.push_back( tce );
            }
         }
      }
   }
}
//---------------------------------------------------------------------------
MinosConfig::~MinosConfig()
{
   if ( !terminated )
      stop();

   elelist.clear();
}
//---------------------------------------------------------------------------
void MinosConfig::saveAll()
{
    {
        QSettings config(getConfigIniName(), QSettings::IniFormat);

        config.clear();

        for ( QVector <QSharedPointer<TConfigElement> >::iterator i = elelist.begin(); i != elelist.end(); i++ )
        {
            (*i)->save(config);
        }
        config.setValue("Settings/ServerName", thisServerName);
        config.setValue( "Settings/HideServers", hideServers );
        config.setValue( "Settings/AutoStart", autoStart );

        config.sync();
    }
    #ifdef Q_OS_UNIX
    sync();         // as just turning machine off can clear the ini file
    #endif
}
void MinosConfig::start()
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
   setHideServers(hideServers);
}

void MinosConfig::stop()
{
   if ( !configExists )
   {
      return ;
   }
   terminated = true;
//   signalCloseEvent();

   for ( QVector <QSharedPointer<TConfigElement> >::iterator i = elelist.begin(); i != elelist.end(); i++ )
   {
      if ( ( *i ) )
      {
         logMessage( "Killing subProcess", "" );
         ( *i ) ->stopProcess();
         logMessage( "subProcess killed", "" );
      }
   }
}
void MinosConfig::setThisServerName( const QString &circle )
{
   thisServerName = circle;
}

bool MinosConfig::getAutoStart()
{
   return autoStart;
}
void MinosConfig::setAutoStart(bool s)
{
    autoStart = s;
}
bool MinosConfig::getHideServers()
{
   return hideServers;
}
void MinosConfig::setHideServers(bool s)
{
    hideServers = s;

    for ( QVector <QSharedPointer<TConfigElement> >::iterator i = elelist.begin(); i != elelist.end(); i++ )
    {
        if (hideServers)
            (*i)->sendCommand("HideServers");
        else
            (*i)->sendCommand("ShowServers");
    }
}

Connectable MinosConfig::getApp(AppType a, QString appName)
{
    Connectable res;
    for ( QVector <QSharedPointer<TConfigElement> >::iterator i = elelist.begin(); i != elelist.end(); i++ )
    {
        if ((*i)->appType == a)
        {
            if (appName.compare((*i)->name, Qt::CaseInsensitive) == 0)
            {
                res = (*i)->connectable();
                break;
            }
        }
    }
    return res;
}
