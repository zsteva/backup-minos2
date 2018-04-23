#include "serialtvswitch.h"


SerialTVSwitch::SerialTVSwitch(QString comport, QObject *parent) : QObject(parent)
{
    sComPort = new QSerialPort(comport, parent);
    // Default to 9600 baud, 8 data, 1 stop, no flow control
    tVSwPort = new SerialComms(sComPort, parent);
}


void SerialTVSwitch::sendTVSwMessage(QByteArray msg)
{
    tVSwPort->write(msg);
}



void SerialTVSwitch::closeComport()
{
    tVSwPort->close();
}
