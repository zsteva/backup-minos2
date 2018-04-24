/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2018
//
//
//
/////////////////////////////////////////////////////////////////////////////


#include <QFile>
#include "serialcomms.h"


SerialComms::SerialComms(QSerialPort *serialPort, QObject *parent):
    QObject(parent),
    m_serialPort(serialPort)

{



    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialComms::handleReadyRead);

    //connect(m_serialPort, &QSerialPort::errorOccurred, this, &SerialComms::handleErrorRead);
    connect(m_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));
    connect(&rd_timer, &QTimer::timeout, this, &SerialComms::handleTimeoutRead);

    connect(m_serialPort, &QSerialPort::bytesWritten, this, &SerialComms::handleBytesWritten);

    connect(&wrt_timer, &QTimer::timeout, this, &SerialComms::handleTimeoutWrite);

    rd_timer.start(5000);


    wrt_timer.setSingleShot(true);


}


/********************* Rx needs more work! ****************************/

void SerialComms::handleReadyRead()
{
    m_readData.append(m_serialPort->readAll());
    if (m_readData.contains(msgTermChar))
    {
        emit msgReceived();
    }
    if (!rd_timer.isActive())
    {
        rd_timer.start(5000);
    }
}



void SerialComms::handleTimeoutRead()
{
    if (m_readData.isEmpty())
    {
        emit noDataRxTimeout(m_serialPort->portName());

    }


}

void SerialComms::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError)
    {
        //emit serialPortError(m_serialPort->portName(), m_serialPort->errorString() );

    }
}






void SerialComms::handleBytesWritten(qint64 bytes)
{
    m_bytesWritten += bytes;
    if (m_bytesWritten == m_writeData.count())
    {
        m_bytesWritten = 0;
        emit dataSuccesfullySent(m_serialPort->portName());

    }
}



void SerialComms::handleTimeoutWrite()
{
    emit writeTimeout(m_serialPort->portName(), m_serialPort->errorString());
}




void SerialComms::write(QByteArray writeData)
{
    m_writeData = writeData;

    const qint64 bytesWritten = m_serialPort->write(writeData);

    if (bytesWritten == -1)
    {
        emit failedToWriteData(m_serialPort->portName(), m_serialPort->errorString());

    } else if (bytesWritten != m_writeData.count())
    {
        emit failToWriteAllData(m_serialPort->portName(), m_serialPort->errorString());

    }

   wrt_timer.start(5000);
}


void SerialComms::close()
{
    m_serialPort->close();
}


void SerialComms::open()
{
    m_serialPort->open(QIODevice::ReadWrite);
}

void SerialComms::setMsgTermChar(char c)
{

    msgTermChar = c;
}
