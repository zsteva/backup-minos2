#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTcpSocket>
#include <QHostAddress>

#include "omnirig.h"

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
    void handle_visibleChange();
    void handle_rigTypeChange(int);
    void handle_statusChange(int);
    void handle_paramsChange(int, int);

private slots:
    void on_selectButton_clicked();

    void on_connectQS1RButton_clicked();

    void on_closeButton_clicked();

    void on_transfer12Button_clicked();

    void on_transfer21Button_clicked();

    void timer1Timeout();
    void timer2Timeout();

    void onSocketConnect();
    void onSocketDisconnect();
    void onError(QAbstractSocket::SocketError);
    void onReadyRead();

private:
    Ui::MainWindow *ui;

    QTimer timer1;
    QTimer timer2;

    QTcpSocket ClientSocket1;

    QScopedPointer<OmniRig::OmniRigX> omni_rig;

    bool muted;
    long getQS1RFreq();
    QString lastF;
    int fCentre;
    int ftf;
};

#endif // MAINWINDOW_H
