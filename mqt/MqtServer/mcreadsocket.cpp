/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#include "minos_pch.h"
#include "mcreadsocket.h"
#include "MServerZConf.h"

MCReadSocket::MCReadSocket(QObject *parent) : QObject(parent)
{

}
MCReadSocket::~MCReadSocket()
{

}

void MCReadSocket::onTimeout()
{

}

void MCReadSocket::setupRO()
{
    qus = QSharedPointer<QUdpSocket>(new QUdpSocket);

    connect(qus.data(), SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChangeRO(QAbstractSocket::SocketState)));

    /*bool res =*/ qus->bind(
        TZConf::getZConf()->groupAddress, //QHostAddress::AnyIPv4,
        UPNP_PORT,
        QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint
        );
}
void MCReadSocket::onSocketStateChangeRO (QAbstractSocket::SocketState state)
{
    if ( state == QAbstractSocket::BoundState )
    {
        /*
        bool res = qus->joinMulticastGroup(TZConf::getZConf()->groupAddress, qui);
        if (res)
        {
            trace("Bound and Joined multicast group " + TZConf::getZConf()->groupAddress.toString()
                  + " on " + qus->localAddress().toString() + " interface :" + qui.humanReadableName());
        }
        else
        {
            trace("Failed to Join multicast group " + TZConf::getZConf()->groupAddress.toString()
                  + " on " + qus->localAddress().toString()
                  + " interface :" + qui.humanReadableName()
                  + " err " + QString::number(qus->error()));
        }
        */

        QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();

        // Interfaces iteration
        for (int i = 0; i < ifaces.size(); i++)
        {
            if (!ifaces[i].flags().testFlag( QNetworkInterface::IsUp))
                continue;
            if (!ifaces[i].flags().testFlag(QNetworkInterface::IsRunning))
                continue;

            QList<QNetworkAddressEntry> addrs = ifaces[i].addressEntries();

            // And for any IP address, if it is IPv4 and the interface is active, make a socket
            for (int j = 0; j < addrs.size(); j++)
            {
                if ((addrs[j].ip().protocol() == QAbstractSocket::IPv4Protocol)
                        && !addrs[j].ip().toString().isEmpty())
                {
                    bool res = qus->joinMulticastGroup(TZConf::getZConf()->groupAddress, ifaces[i]);
                    if (res)
                    {
                        trace("Bound and Joined multicast group " + TZConf::getZConf()->groupAddress.toString()
                              + " on " + qus->localAddress().toString() + " interface :" + ifaces[i].humanReadableName());
                    }
                    else
                    {
                        trace("Failed to Join multicast group " + TZConf::getZConf()->groupAddress.toString()
                              + " on " + qus->localAddress().toString()
                              + " interface :" + ifaces[i].humanReadableName()
                              + " err " + QString::number(qus->error()));
                    }
                    break;
                }
            }


        }

        connect(qus.data(), SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    }
}


void MCReadSocket::onReadyRead()
{
    while (qus->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(qus->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        qus->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        trace("DATAGRAM READ : "
             + QString(datagram) + " from " + sender.toString() + " on " + ifaceName);

        emit readyRead(QString(datagram), sender.toString());

    }
}
