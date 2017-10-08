/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rig Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2017
//
//
//
//
/////////////////////////////////////////////////////////////////////////////



#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <QStringList>
#include "rigcontrol.h"

#define NUM_RADIOS 5


class QIntValidator;
class QComboBox;
class QLineEdit;
class QCheckBox;
class QRadioButton;



namespace Ui {
class SetupDialog;
}




class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDialog(RigControl *rig, QWidget *parent = 0);
    ~SetupDialog();


    scatParams getCurrentRadio() const;
    int getRadioId(QString rotator);


    scatParams availRadios[NUM_RADIOS];
    scatParams currentRadio;



    QString getRadioProtocol(QString );
    void readCurrentRadio();
    QString getRadioComPort(QString);
    void saveCurrentRadio();

    void copyRadioToCurrent(int radioNumber);

    void readSettings();
    void setAppName(QString name);
signals:

    void radioNameChange();
    void currentRadioSettingChanged(QString);

private slots:

    void radioNameFinished(int);
    void radioModelSelected(int);
    void civAddressFinished(int);
    void comportSelected(int);
    void comSpeedSelected(int);
    void comDataBitsSelected(int);
    void comStopBitsSelected(int);
    void comParitySelected(int);
    void comHandShakeSelected(int);
    void transVertChecked(int);
    void transVertEditFinished(int);

    void saveButtonPushed();
    void cancelButtonPushed();
    void transNegChecked(int boxNumber);
    void rxPassBandChecked(int boxNumber);

    void networkAddressSelected(int boxNumber);
    void networkPortSelected(int boxNumber);
private:
    Ui::SetupDialog *ui;

    RigControl *radio;

    QLineEdit *radioName[NUM_RADIOS];
    QComboBox *radioModel[NUM_RADIOS];
    QLineEdit *civAddress[NUM_RADIOS];

    QComboBox *comPorts[NUM_RADIOS];
    QLabel *comPortLbl[NUM_RADIOS];

    QComboBox *comSpeed[NUM_RADIOS];
    QLabel *comSpeedLbl[NUM_RADIOS];

    QComboBox *comDataBits[NUM_RADIOS];
    QLabel *comDataLbl[NUM_RADIOS];

    QComboBox *comStopBits[NUM_RADIOS];
    QLabel *comStopLbl[NUM_RADIOS];

    QComboBox *comParity[NUM_RADIOS];
    QLabel *comParityLbl[NUM_RADIOS];

    QComboBox *comHandShake[NUM_RADIOS];
    QLabel *comHandLbl[NUM_RADIOS];

    QLineEdit *netAddress[NUM_RADIOS];
    QLabel *netAddLbl[NUM_RADIOS];

    QLineEdit *netPort[NUM_RADIOS];
    QLabel *netPortLbl[NUM_RADIOS];


    QCheckBox *transVertCheck[NUM_RADIOS];
    QLineEdit *transVertEdit[NUM_RADIOS];
    QCheckBox *transNegCheck[NUM_RADIOS];
    QCheckBox *rxPassBandCheck[NUM_RADIOS];
//    QIntValidator *intValidator;
    bool radioValueChanged[NUM_RADIOS];
    bool radioNameChanged[NUM_RADIOS];
    bool radioChanged;
    bool chkloadflg = false;
    QString appName = "";
    void saveSettings();

    void fillRadioModelInfo();
    void fillPortsInfo();
    void fillSpeedInfo();
    void fillDataBitsInfo();
    void fillStopBitsInfo();
    void fillParityInfo();
    void fillHandShakeInfo();

    void clearAvailRadio();
    void clearCurrentRadio();
    //    void enableCIVbox();
    void clearRadioValueChanged();
    void clearRadioNameChanged();
    void civSetToolTip();
    void networkDataEntryVisible(int radioNumber, bool visible);
    void serialDataEntryVisible(int radioNumber, bool visible);
};

#endif // SETUPDIALOG_H
