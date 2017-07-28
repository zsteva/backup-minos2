#ifndef CONFIGFILE_H
#define CONFIGFILE_H
#include <QWidget>
#include <QProcess>
#include <QSettings>
//---------------------------------------------------------------------------
extern QString RunTypeNone;
extern QString RunLocal;
extern QString ConnectLocal;
extern QString ConnectServer;

extern bool checkGoodRunType(const QString &s);

class AppConfigElement
{
public:
    AppConfigElement(){}

    QString appType;
    QString appPath;
    QStringList requires;
    bool server;
};

class Connectable
{
public:
    Connectable(){}

    QString serverName;
    QString remoteAppName;
    QString appName;
    QString runType;
    QString appType;
};

class RunConfigElement: public QObject
{
    Q_OBJECT
private:  	// User declarations
    QProcess *runner;
public:  		// User declarations
    QString name;
    QString rundir;
    QString commandLine;
    QString params;
    QString server;
    QString remoteApp;

    QString runType;
    QString appType;

    QStringList requires;

    bool stopping;

    RunConfigElement();
    bool initialise( QSettings &, QString sect );

    void save(QSettings &);

    Connectable connectable();

    void createProcess();
    void stopProcess();
    void sendCommand(const QString & cmd);
private slots:
    void	on_started();
    void    on_finished(int, QProcess::ExitStatus exitStatus);
    void	on_error(QProcess::ProcessError error);

    void	on_readyReadStandardError();
    void	on_readyReadStandardOutput();
};
class MinosConfig : public QObject
{
    Q_OBJECT
private:  	// User declarations
    static MinosConfig *thisDM;
    static QString getConfigIniName();
    MinosConfig();
    void initialise();

    QVector<AppConfigElement> appConfigList;

    void buildAppConfigList();

    QString thisServerName;
    bool hideServers;
    bool autoStart;
public:  		// User declarations
    static MinosConfig *getMinosConfig( );

    ~MinosConfig();

    QVector <QSharedPointer<RunConfigElement> > elelist;

    static QString getThisServerName();

    QStringList getAppTypes();
    Connectable getApp(QString appName);
    AppConfigElement getAppConfigElement(QString appType);

    void saveAll();

    void start();
    void stop();
    void setThisServerName( const QString &circle );

    bool getAutoStart();
    void setAutoStart(bool);
    bool getHideServers();
    void setHideServers(bool);

    QString checkConfig();

};
//---------------------------------------------------------------------------

#endif // CONFIGFILE_H
