

#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>

#include "rigcommon.h"

rigcommon::rigcommon()
{

}


void fillPortsInfo(QComboBox* comportSel)
{

    comportSel->clear();

    QString description;
    QString manufacturer;
    QString serialNumber;

    comportSel->addItem("");

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);


        comportSel->addItem(list.first(), list);

    }


}
