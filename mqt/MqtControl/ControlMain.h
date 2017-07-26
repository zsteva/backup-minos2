#ifndef CONTROLMAIN_H
#define CONTROLMAIN_H

#include "base_pch.h"
#include <QMainWindow>
#include "CommonMonitor.h"
#include "controlport.h"

namespace Ui {
class ControlMain;
}

class ControlMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit ControlMain(QWidget *parent = 0);
    ~ControlMain();
    commonController monitor;

    void setPTTIn(bool s);
    void setPTTOut(bool s);
    void setL1(bool s);
    void setL2(bool s);
    void setL3(bool s);
    void setL4(bool s);

private:
    Ui::ControlMain *ui;
    void logMessage( QString s );
    void doProcLineEvents( );
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void changeEvent( QEvent* e ) override;

    StdInReader stdinReader;
    QTimer LogTimer;
    QTimer formShowTimer;

private slots:
    void onStdInRead(QString);

    void on_formShown();
    void controlClientCallback( bool /*err*/, QSharedPointer<MinosRPCObj>/*mro*/, const QString &/*from*/ );
    void controlServerCallback(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from );
    void LogTimerTimer( );
    void linesChangedEvent();
};

#endif // CONTROLMAIN_H
