#include "mqtUtils_pch.h"

#include <QSettings>
#include <QHostInfo>
#include "fileutils.h"
#include "ConfigFile.h"

//---------------------------------------------------------------------------

bool terminated = false;
bool configExists = false;
static QStringList runTypes{"None", "RunLocal", "ConnectLocal", "ConnectServer"};
static QStringList appTypes{
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
    return appTypes;
}

//---------------------------------------------------------------------------
TConfigElement::TConfigElement()
      : runType( rtNone ), appType(atOther), runner(0)
{}
//---------------------------------------------------------------------------
bool TConfigElement::initialise( QSettings &config, QString sect )
{
    if (!configExists)
        return false;

    name = sect;

    commandLine = config.value( sect + "/Program", "" ).toString().trimmed();
    server = config.value( sect + "/Server", "localhost" ).toString().trimmed();
    params = config.value( sect + "/Params", "" ).toString();
    rundir = config.value( sect + "/Directory", "" ).toString().trimmed();

    QString S = config.value( sect + "/RunType", runTypes[rtNone] ) .toString().trimmed();

    runType = rtNone;
    for (int i = 0; i < rtMax; i++)
    {
        if (S.compare(runTypes[i], Qt::CaseInsensitive) == 0)
        {
            runType = static_cast<RunType>(i);
            break;
        }

    }

    S = config.value( sect + "/AppType", appTypes[atOther] ) .toString().trimmed();

    appType = atOther;
    for (int i = 0; i < atMax; i++)
    {
        if (S.compare(appTypes[i], Qt::CaseInsensitive) == 0)
        {
            appType = static_cast<AppType>(i);
            break;
        }
    }
    return true;
}
void TConfigElement::save(QSettings &config)
{
    if (!name.isEmpty() && name.compare("<Deleted>", Qt::CaseInsensitive) != 0)
    {
        config.setValue(name + "/Program", commandLine);
        config.setValue(name + "/Params", params);
        config.setValue(name + "/Directory", rundir);
        config.setValue(name + "/Server", server);
        config.setValue(name + "/RunType", runTypes[runType]);
        config.setValue(name + "/AppType", appTypes[appType]);
    }
}
Connectable TConfigElement::connectable()
{
    Connectable res;
    res.appName = name;
    res.appType = appType;
    res.runType = runType;
    if (runType == rtConnectServer)
        res.serverName = server;
    else
        res.serverName = name + "@localhost";

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

        // pass the RPC name to those apps that can understand it
        // and we can add in hideServers, and anything else useful
        // ?? simple text, XML/XMPP or JSON ??
        // Whatever, we need a standard startup routine to be linked into everything
        // keep the link pen - we may want to send hideServers etc later
        QByteArray command = (name + "\nCloseStdin\n").toUtf8();
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
    runner->start();    // but we have to be careful when we close!
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
}

void MinosConfig::stop()
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
}

Connectable MinosConfig::getApp(AppType a, QString appName)
{
    Connectable res;
    for ( QVector <QSharedPointer<TConfigElement> >::iterator i = elelist.begin(); i != elelist.end(); i++ )
    {
        if ((*i)->appType == a)
        {
            res = (*i)->connectable();
            break;
        }
    }
    return res;
}
