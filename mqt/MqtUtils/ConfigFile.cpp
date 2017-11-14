#include "mqtUtils_pch.h"

#include <QTimer>
#include <QSettings>
#include <QHostInfo>
#include "fileutils.h"
#include "ConfigFile.h"


//---------------------------------------------------------------------------

bool terminated = false;

QString RunLocal("RunLocal");
QString ConnectServer("ConnectServer");


/*static*/
MinosConfig *MinosConfig::thisDM = 0;
/*static*/
MinosConfig *MinosConfig::getMinosConfig()
{
   if ( !thisDM )
   {
      thisDM = new MinosConfig();
      thisDM->initialise();
   }
   return thisDM;
}
/*static*/
QString MinosConfig::getConfigIniName()
{
    return "./Configuration/MinosConfig.ini";
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
void MinosConfig::cleanElementsOnCancel()
{
    for ( QVector <QSharedPointer<RunConfigElement> >::iterator i = elelist.begin(); i != elelist.end(); i++ )
    {
        QSharedPointer<RunConfigElement> ele = (*i);
        if (ele->newElement)
        {
            ele->name = "<Deleted>";
        }
    }
}

//---------------------------------------------------------------------------
bool RunConfigElement::initialise( QSettings &config, QString sect )
{
    // config should refer to ./Configuration/MinosConfig.ini

    name = sect;

    commandLine = config.value( sect + "/Program", "" ).toString().trimmed();
    server = config.value( sect + "/Server", "localhost" ).toString().trimmed();
    params = config.value( sect + "/Params", "" ).toString().trimmed();
    rundir = config.value( sect + "/Directory", "" ).toString().trimmed();
    remoteApp = config.value(sect + "/RemoteApp", name).toString().trimmed();
    showAdvanced = config.value(sect + "/ShowAdvanced", false).toBool();
    rEnabled = config.value(sect + "/Enabled", false).toBool();
    hideApp = config.value(sect + "/HideApp", false).toBool();
    QString S = config.value( sect + "/RunType", RunLocal ) .toString().trimmed();

    runType = S;

    appType = config.value( sect + "/AppType", QString() ) .toString().trimmed();

    AppConfigElement ace = MinosConfig::getMinosConfig()->getAppConfigElement(appType);
    requires = ace.requires;
    localOK = ace.localOK;
    remoteOK = ace.remoteOK;

    return true;
}
void RunConfigElement::save(QSettings &config)
{
    newElement = false;

    if (name.isEmpty())
    {
        name = appType;
    }

    if (name.compare("<Deleted>", Qt::CaseInsensitive) != 0)
    {
        config.setValue(name + "/Program", commandLine);
        config.setValue(name + "/Params", params);
        config.setValue(name + "/Directory", rundir);
        config.setValue(name + "/Server", server);
        config.setValue(name + "/RemoteApp", remoteApp);
        config.setValue(name + "/RunType", runType);
        config.setValue(name + "/AppType", appType);
        config.setValue(name + "/ShowAdvanced", showAdvanced);
        config.setValue(name + "/Enabled", rEnabled);
        config.setValue(name + "/HideApp", hideApp);
    }
}
Connectable RunConfigElement::connectable()
{
    Connectable res;
    res.appName = name;
    res.appType = appType;
    res.runType = runType;
    if (runType == ConnectServer)
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

void RunConfigElement::createProcess()
{
    if (name.compare("<Deleted>", Qt::CaseInsensitive) == 0)
        return;
    if (rEnabled && runType == RunLocal && !runner)
    {
        runner = new QProcess(parent());

        QString program = commandLine;
        if (!FileExists(program))
        {
            trace(name + ":program doesn't exist:" + program);
        }

        program += " ";
        program += params;

        QString wdir = rundir;
        runner->setWorkingDirectory(wdir);

        if (appType != "None" )
        {
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            env.insert("MQTRPCNAME", name); // Add an environment variable for the RPC name to use
            runner->setProcessEnvironment(env);
        }

        connect (runner, SIGNAL(started()), this, SLOT(on_started()));
        connect (runner, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_finished(int, QProcess::ExitStatus)));
        connect (runner, SIGNAL(error(QProcess::ProcessError)), this, SLOT(on_error(QProcess::ProcessError)));

        connect (runner, SIGNAL(readyReadStandardError()), this, SLOT(on_readyReadStandardError()));
        connect (runner, SIGNAL(readyReadStandardOutput()), this, SLOT(on_readyReadStandardOutput()));

        runner->start(program);

        if (hideApp)
            sendCommand("HideServers");
        else
            sendCommand("ShowServers");


    }
}
void RunConfigElement::stopProcess()
{
    if (runner)
    {
        stopping = true;
        runner->terminate();
    }
}

void RunConfigElement::sendCommand(const QString & cmd)
{
    if (runner && appType != "None" )
    {
        QByteArray command = (cmd + "\n").toUtf8();
        qint64 res = runner->write( command );
        if (res < 0)
        {
            trace("Failed to write " + cmd + " to runner");
        }
        else
        {
            trace("Wrote " + cmd + " to runner");
        }
    }
}

void RunConfigElement::on_started()
{
    trace(name + ":started");
}

void RunConfigElement::on_finished(int err, QProcess::ExitStatus exitStatus)
{
    trace(name + ":finished:" + QString::number(err) + ":" + QString::number(exitStatus));
    if (runner)
    {

        runner->closeWriteChannel();
        runner->deleteLater();
        runner = 0;
        if (stopping)
        {
            stopping = false;

        }
        else
        {
            createProcess();
//            runner->start();    // but we have to be careful when we close!
        }
    }
}

void RunConfigElement::on_error(QProcess::ProcessError error)
{
    trace(name + ":error:" + QString::number(error));
}

void RunConfigElement::on_readyReadStandardError()
{
    if (runner)
    {
        QString r = runner->readAllStandardError();
        trace(name + ":stdErr:" + r);
    }
}

void RunConfigElement::on_readyReadStandardOutput()
{
    if (runner)
    {
        QString r = runner->readAllStandardOutput();
        trace(name + ":stdOut:" + r);
    }
}

//---------------------------------------------------------------------------
MinosConfig::MinosConfig( )
    : QObject( 0 ), autoStart(false)
{
}
void MinosConfig::initialise()
{
    buildAppConfigList();
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
        }
        else
        {
            QSharedPointer<RunConfigElement> tce = QSharedPointer<RunConfigElement>(new RunConfigElement());
            if ( tce->initialise( config, sect ) )
            {
                elelist.push_back( tce );
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
        config.sync();
    }
    {
        QSettings config(getConfigIniName(), QSettings::IniFormat);
        for ( QVector <QSharedPointer<RunConfigElement> >::iterator i = elelist.begin(); i != elelist.end(); i++ )
        {
            (*i)->save(config);
        }
        config.setValue("Settings/ServerName", thisServerName);
        config.setValue( "Settings/AutoStart", autoStart );

        config.sync();
    }
    #ifdef Q_OS_UNIX
    sync();         // as just turning machine off can clear the ini file
    #endif
}
void MinosConfig::start()
{
   terminated = false;

   for ( QVector <QSharedPointer<RunConfigElement> >::iterator i = elelist.begin(); i != elelist.end(); i++ )
   {
       (*i)->createProcess();
   }
}

void MinosConfig::stop()
{
   terminated = true;

   for ( QVector <QSharedPointer<RunConfigElement> >::iterator i = elelist.begin(); i != elelist.end(); i++ )
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
Connectable MinosConfig::getApp(QString appName)
{
    Connectable res;
    for ( QVector <QSharedPointer<RunConfigElement> >::iterator i = elelist.begin(); i != elelist.end(); i++ )
    {
        if (appName.compare((*i)->name, Qt::CaseInsensitive) == 0)
        {
            res = (*i)->connectable();
            break;
        }
    }
    return res;
}

QStringList MinosConfig::getAppTypes()
{
    QStringList apps;
    for (int i = 0; i < appConfigList.size(); i++)
    {
        apps.append(appConfigList[i].appType);
    }
    apps.sort();
    apps.insert(0, "None");
    apps.removeDuplicates();
    return apps;
}
void MinosConfig::buildAppConfigList()
{
    QSettings appConfig("./Configuration/AppConfig.ini", QSettings::IniFormat);
    /*
[BandMap]
Path=./mqtBandMap
Enabled=false
Requires=Server
Server=false

   */
    QStringList apps = appConfig.childGroups();
    for (int i = 0; i < apps.size(); i++)
    {
        if (appConfig.value(apps[i] + "/Enabled", false).toBool())  // only include those elements we are allowed to as possibilities
        {
            AppConfigElement ac;

            ac.appType = apps[i].trimmed();
            ac.appPath = appConfig.value(apps[i] + "/Path").toString().trimmed();
#ifdef Q_OS_WIN
            ac.appPath += ".exe";
#endif
            ac.server = appConfig.value(apps[i] + "/Server", false).toBool();
            ac.defaultHide = appConfig.value(apps[i] + "/HideApp", false).toBool();

            QString whereString = appConfig.value(apps[i] + "/Where", "Remote,Local").toString();
            if (whereString.contains("local", Qt::CaseInsensitive))
            {
                ac.localOK = true;
            }
            else
            {
                ac.localOK = false;
            }
            if (whereString.contains("remote", Qt::CaseInsensitive))
            {
                ac.remoteOK = true;
            }
            else
            {
                ac.remoteOK = false;
            }


            // NB using comma in value give a string list! Single value will also go to list if desired
            ac.requires = appConfig.value(apps[i] + "/Requires").toStringList();

            for(auto& str : ac.requires)    // trim all elements of leading and trailing spaces
                str = str.trimmed();

            appConfigList.append(ac);
        }
    }

}
QString MinosConfig::checkConfig()
{
    QString reqErrs;

    bool serverPresent = false;
    int eleListSize = 0;
    for ( QVector <QSharedPointer<RunConfigElement> >::iterator i = elelist.begin(); i != elelist.end(); i++ )
    {
        QSharedPointer<RunConfigElement> ele = (*i);
        if (ele->name.compare("<Deleted>", Qt::CaseInsensitive) == 0)
            continue;
        if (ele->rEnabled)
        {
            eleListSize++;
            if (ele->appType == "Server" && ele->runType == RunLocal )
            {
                serverPresent = true;
                break;
            }
        }
    }

    if (eleListSize && !serverPresent)
    {
        reqErrs += "A local server is required.\r\n\r\n" ;
    }

    //Check that the name is not blank, and only has allowed characters
    //Check that the names aren't duplicates

    // Go through the configured elements, and check that their requirements are also present
    for ( QVector <QSharedPointer<RunConfigElement> >::iterator i = elelist.begin(); i != elelist.end(); i++ )
    {
        QSharedPointer<RunConfigElement> ele = (*i);

        if (ele->name.compare("<Deleted>", Qt::CaseInsensitive) == 0)
            continue;

        if (ele->rEnabled)
        {
            if ( ele->requires.size() > 0 && ele->runType == RunLocal)
            {
                // "Requires" elements must be present
                foreach(QString req, ele->requires)
                {
                    if (req.isEmpty())
                        continue;

                    if (req == "Server")
                        continue;


                    bool reqFound = false;
                    for ( QVector <QSharedPointer<RunConfigElement> >::iterator j = elelist.begin(); j != elelist.end(); j++ )
                    {
                        if ((*j)->name.compare("<Deleted>", Qt::CaseInsensitive) == 0)
                            continue;
                        if ((*j)->appType == req && (*j)->rEnabled && (*j)->runType == RunLocal)
                        {
                            reqFound = true;
                            continue;
                        }
                    }
                    if (!reqFound)
                    {
                        reqErrs += ele->appType + " requires a local " + req + "\n\n";
                    }
                }
            }


            if (ele->runType == RunLocal)
            {
                if (!FileExists(ele->commandLine))
                {
                    reqErrs += ele->appType + " Executable path does not exist\n\n";
                }
                if (ele->appType != "None" && !FileExists(ele->rundir + "/Configuration/MinosConfig.ini"))
                {
                    reqErrs += ele->appType + " Working directory is not valid - no Configuration/MinosConfig.ini\n\n";
                }
            }
        }
    }
    return reqErrs;
}
AppConfigElement MinosConfig::getAppConfigElement(QString appType)
{
    AppConfigElement ace;
    for ( QVector <AppConfigElement >::iterator j = appConfigList.begin(); j != appConfigList.end(); j++ )
    {
        if ((*j).appType == appType)
        {
            ace = (*j);
            break;
        }
    }
    return ace;
}
