#ifndef CONFIGFILE_H
#define CONFIGFILE_H
#include <QWidget>
#include <QProcess>
#include <QSettings>
//---------------------------------------------------------------------------
enum RunType{rtNone, rtLocal, rtServer, rtMax};
enum AppType{atOther, atServer, atAppStarter, atLogger, atChat, atKeyer, atBandMap, atRotator, atMonitor, atRotatorControl, atRigControl, atMax};

class Connectable
{
public:
    QString serverName;
    QString appName;
    RunType runType;
    AppType appType;
};

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

    RunType runType;
    AppType appType;

    TConfigElement();
    bool initialise( QSettings &, QString sect );

    void save(QSettings &);

    Connectable connectable();

    void createProcess();
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

    QVector <QProcess *> guardv;
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
