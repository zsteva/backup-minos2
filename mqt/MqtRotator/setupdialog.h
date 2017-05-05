/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
//
/////////////////////////////////////////////////////////////////////////////



#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QStringList>
#include "rotcontrol.h"

#define NUM_ROTATOR_PROTOCOLS 2
#define NUM_ANTENNAS 5


class QIntValidator;
class QComboBox;
class QLineEdit;
class QCheckBox;



namespace Ui {
class SetupDialog;
}




class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDialog(RotControl *rotator, QWidget *parent = 0);
    ~SetupDialog();


    srotParams getCurrentAntenna() const;
    int getRotatorId(QString rotator);


    srotParams availAntennas[NUM_ANTENNAS];
    srotParams currentAntenna;



    QString getRotatorProtocol(QString );
    void readCurrentAntenna();
    QString getRotatorComPort(QString);
    void saveCurrentAntenna();

protected slots:

    void antennaNameFinished(int);
    void rotatorModelSelected(int);
    void southStopFlagSelected(int);
    void overRunFlagSelected(int);
    void rotatorOffsetFinished(int);
    void comportSelected(int);
    void comSpeedSelected(int);
    void comDataBitsSelected(int);
    void comStopBitsSelected(int);
    void comParitySelected(int);
    void comHandShakeSelected(int);

    void saveButtonPushed();
    void cancelButtonPushed();


private:
    Ui::SetupDialog *ui;

    RotControl *rotator;

    QLineEdit *antennaName[NUM_ANTENNAS];
    QComboBox *rotatorModel[NUM_ANTENNAS];
    QCheckBox *southStopFlag[NUM_ANTENNAS];
    QCheckBox *overRunFlag[NUM_ANTENNAS];
    QLineEdit *rotatorOffset[NUM_ANTENNAS];
    QComboBox *comPorts[NUM_ANTENNAS];
    QComboBox *comSpeed[NUM_ANTENNAS];
    QComboBox *comDataBits[NUM_ANTENNAS];
    QComboBox *comStopBits[NUM_ANTENNAS];
    QComboBox *comParity[NUM_ANTENNAS];
    QComboBox *comHandShake[NUM_ANTENNAS];
    QIntValidator *intValidator;
    bool antennaValueChanged[NUM_ANTENNAS];
    bool antennaChanged;
    void saveSettings();
    void readSettings();
    void fillAntennaModelInfo();
    void fillPortsInfo();
    void fillSpeedInfo();
    void fillDataBitsInfo();
    void fillStopBitsInfo();
    void fillParityInfo();
    void fillHandShakeInfo();
    void clearAvailRotators();
    void clearCurrentRotator();
    void initRotProtocols();




};

#endif // SETUPDIALOG_H