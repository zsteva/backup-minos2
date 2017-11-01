#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "base_pch.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

private slots:
    void on_closeButton_clicked();
    void on_transfer12Button_clicked();
    void on_transfer21Button_clicked();

    void SyncTimerTimer();
    void timer2Timeout();

    void onStdInRead(QString);

    void onSocketConnect();
    void onSocketDisconnect();
    void onError(QAbstractSocket::SocketError);
    void onReadyRead();

    void on_serverCall( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );
    void on_notify( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from );

private:
    Ui::MainWindow *ui;

    virtual void closeEvent(QCloseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void changeEvent( QEvent* e ) override;

    StdInReader stdinReader;

    QTimer SyncTimer;
    QTimer timer2;

    QTcpSocket ClientSocket1;

    bool qs1rConnected = false;

    bool muted;
    long getQS1RFreq();
    QString lastF;
    int fCentre;
    int ftf;

    QString state;
    QString mode;
    QString freq;
    QString radioName;

};

#endif // MAINWINDOW_H
