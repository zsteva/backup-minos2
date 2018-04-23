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
class UPnPDataObject;

class MCReadSocket : public QObject
{
    Q_OBJECT
    QSharedPointer<QUdpSocket> qus;
    QString ifaceName;
    QNetworkInterface qui;
    UPnPDataObject *state;

public:
    explicit MCReadSocket(QObject *parent = nullptr);
    virtual ~MCReadSocket() override;
    bool setupRO();

    void onTimeout();
signals:

    void readyRead(QString, QString);

public slots:
    //void onSocketStateChangeRO(QAbstractSocket::SocketState);
    //void onReadyRead();
}
;

#endif // MCREADSOCKET_H
