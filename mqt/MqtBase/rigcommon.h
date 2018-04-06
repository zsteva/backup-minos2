#ifndef RIGCOMMON_H
#define RIGCOMMON_H

#include <QComboBox>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>


static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

class rigcommon
{
public:
    rigcommon();
};




void fillPortsInfo(QComboBox* comportSel);


#endif // RIGCOMMON_H
