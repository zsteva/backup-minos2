#ifndef CONFIGFILE_H
#define CONFIGFILE_H
#include <QWidget>
#include <QProcess>
#include <QSettings>
//---------------------------------------------------------------------------
enum RunType{rtNone, rtRunLocal, rtConnectLocal, rtConnectServer, rtMax};
enum AppType{
    atNone,
    atAppStarter,
    atBandMap,
    atChat,
    atKeyer,
    atLineControl,
    atLogger,
    atMonitor,
    atOther,
    atRigControl,
    atRotator,
    atRotatorControl,
    atServer,
    atMax};

class Connectable
{
public:
    Connectable(): runType(rtNone), appType(atNone)
    {

    }
    QString serverName;
    QString remoteAppName;
    QString appName;
    RunType runType;
    AppType appType;
};
QString getRunType(RunType r);
QString getAppType(AppType r);
QString getDefaultApp(AppType r);

class TConfigElement: public QObject
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

    RunType runType;
    AppType appType;

    bool stopping;

    TConfigElement();
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

    MinosConfig( QWidget* Owner );

    QString thisServerName;
    bool hideServers;
    bool autoStart;
public:  		// User declarations
    ~MinosConfig();

    static bool doesConfigExist();
    static QString getThisServerName();
    static QStringList getAppTypes();

    Connectable getApp(AppType a, QString appName);

    QVector <QSharedPointer<TConfigElement> > elelist;

    void saveAll();

    void start();
    void stop();
    void setThisServerName( const QString &circle );

    bool getAutoStart();
    void setAutoStart(bool);
    bool getHideServers();
    void setHideServers(bool);

    static MinosConfig *getMinosConfig( QWidget* Owner );
};
//---------------------------------------------------------------------------

#endif // CONFIGFILE_H
