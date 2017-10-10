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
#include <QLabel>
#include "rotcontrol.h"
#include "rotatorCommonConstants.h"




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
    void saveCurrentAntenna();
    QString getRotatorComPort(QString);

    void readSettings();
    void copyAntennaToCurrent(int antennaNumber);

    void setAppName(QString name);
signals:

    void currentAntennaSettingChanged(QString);
    void antennaNameChange();


private slots:

    void antennaNameFinished(int);
    void rotatorModelSelected(int);
    void southStopFlagSelected(int);
    void overRunFlagSelected(int);
    void antennaOffsetFinished(int);
    void comportSelected(int);
    void comSpeedSelected(int);
    void comDataBitsSelected(int);
    void comStopBitsSelected(int);
    void comParitySelected(int);
    void comHandShakeSelected(int);

    void saveButtonPushed();
    void cancelButtonPushed();
    void networkAddressSelected(int boxNumber);
    void networkPortSelected(int boxNumber);


private:
    Ui::SetupDialog *ui;

    RotControl *rotator;

    QLineEdit *antennaName[NUM_ANTENNAS];
    QComboBox *rotatorModel[NUM_ANTENNAS];

    QCheckBox *southStopFlag[NUM_ANTENNAS];
    QLabel *southStopLabel[NUM_ANTENNAS];

    QCheckBox *overRunFlag[NUM_ANTENNAS];
    QLabel *overRunLabel[NUM_ANTENNAS];

    QLineEdit *antennaOffset[NUM_ANTENNAS];

    QComboBox *comPorts[NUM_ANTENNAS];
    QLabel *comPortLbl[NUM_ANTENNAS];

    QComboBox *comSpeed[NUM_ANTENNAS];
    QLabel *comSpeedLbl[NUM_ANTENNAS];

    QComboBox *comDataBits[NUM_ANTENNAS];
    QLabel *comDataLbl[NUM_ANTENNAS];

    QComboBox *comStopBits[NUM_ANTENNAS];
    QLabel *comStopLbl[NUM_ANTENNAS];

    QComboBox *comParity[NUM_ANTENNAS];
    QLabel *comParityLbl[NUM_ANTENNAS];

    QComboBox *comHandShake[NUM_ANTENNAS];
    QLabel *comHandLbl[NUM_ANTENNAS];

    QLineEdit *netAddress[NUM_ANTENNAS];
    QLabel *netAddLbl[NUM_ANTENNAS];

    QLineEdit *netPort[NUM_ANTENNAS];
    QLabel *netPortLbl[NUM_ANTENNAS];

//    QIntValidator *intValidator;
    bool antennaValueChanged[NUM_ANTENNAS];
    bool antennaNameChanged[NUM_ANTENNAS];
    bool antennaChanged;
    bool chkloadflg = false;
    QString appName = "";
    const int minOffset = -90;
    const int maxOffset = 90;
    void saveSettings();
    void fillRotatorModelInfo();
    void fillPortsInfo();
    void fillSpeedInfo();
    void fillDataBitsInfo();
    void fillStopBitsInfo();
    void fillParityInfo();
    void fillHandShakeInfo();
    void clearAvailRotators();
    void clearCurrentRotator();
    void initRotProtocols();




    void clearAntennaValueChanged();
    int getMaxMinRotationData(int rotatorNumber, int *maxRot, int *minRot);
    void clearAntennaNameChanged();
    void serialDataEntryVisible(int antNumber, bool visible);
    void networkDataEntryVisible(int antNumber, bool visible);
};

#endif // SETUPDIALOG_H
