#ifndef MCREADSOCKET_H
#define MCREADSOCKET_H

#include <QApplication>
#include <QObject>

#include <QSharedPointer>
#include <qabstractsocket.h>
#include <QUdpSocket>
#include <qnetworkinterface.h>

extern bool closeApp;

#define UPNP_PORT 9999
#define UPNP_GROUP "239.255.0.1"

class MCReadSocket : public QObject
{
    Q_OBJECT
    QSharedPointer<QUdpSocket> qus;
    QString ifaceName;
    QNetworkInterface qui;
public:
    explicit MCReadSocket(QObject *parent = 0);
    virtual ~MCReadSocket() override;
    void setupRO();

    void onTimeout();
signals:

    void readyRead(QString, QString);

public slots:
    void onSocketStateChangeRO(QAbstractSocket::SocketState);
    void onReadyRead();
}
;

#endif // MCREADSOCKET_H
