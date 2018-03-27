#ifndef CLUSTERMAINWINDOW_H
#define CLUSTERMAINWINDOW_H

#include <QMainWindow>
#include "qttelnet.h"
#include "cluster.h"

namespace Ui {
class ClusterMainWindow;
}

class ClusterMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClusterMainWindow(QWidget *parent = 0);
    ~ClusterMainWindow();

private slots:
    void connectionEstab();
    void connectionError(QAbstractSocket::SocketError);
    void messageRx(QString msg);
    void logIn();
    void sendText();
    void parseDX(QString txt);
    void checkedLoggedIn(QString msg);
private:
    Ui::ClusterMainWindow *ui;
    QtTelnet* client;
    Cluster* dxCluster;

    QStringList dxMsg;
    QString dxCall;
    QString dxFreq;
    QString spotCall;
    QString spotComment;
    QString spotTime;
    QString dxLocator;

    bool loginStart;
    bool loginSuccess;
    void txText(QString msg);
    int upackSpot(QString txt);
};

#endif // CLUSTERMAINWINDOW_H
