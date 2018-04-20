#ifndef BMSETUPDIALOG_H
#define SBMETUPDIALOG_H

#include <QDialog>
//#include <QtSerialPort/QSerialPort>
#include <QStringList>
#include "rigcontrol.h"

//#define NUM_ROTATOR_PROTOCOLS 2
#define NUM_RADIOS 5


class QIntValidator;
class QComboBox;
class QLineEdit;



namespace Ui {
class BMSetupDialog;
}




class BMSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BMSetupDialog(RigControl *rig, QWidget *parent = 0);
    ~BMSetupDialog();


    scatParams getCurrentRadio() const;
    int getRadioId(QString rotator);


    scatParams availRadios[NUM_RADIOS];
    scatParams currentRadio;



    QString getRadioProtocol(QString );
    void readCurrentRadio();
    QString getRadioComPort(QString);
    void saveCurrentRadio();

protected slots:

    void radioNameFinished(int);
    void radioModelSelected(int);
    void civAddressFinished(int);
    void comportSelected(int);
    void comSpeedSelected(int);
    void comDataBitsSelected(int);
    void comStopBitsSelected(int);
    void comParitySelected(int);
    void comHandShakeSelected(int);

    void saveButtonPushed();
    void cancelButtonPushed();


private:
    Ui::BMSetupDialog *ui;

    RigControl *radio;

    QLineEdit *radioName[NUM_RADIOS];
    QComboBox *radioModel[NUM_RADIOS];
    QLineEdit *civAddress[NUM_RADIOS];
    QComboBox *comPorts[NUM_RADIOS];
    QComboBox *comSpeed[NUM_RADIOS];
    QComboBox *comDataBits[NUM_RADIOS];
    QComboBox *comStopBits[NUM_RADIOS];
    QComboBox *comParity[NUM_RADIOS];
    QComboBox *comHandShake[NUM_RADIOS];
    QIntValidator *intValidator;
    bool radioValueChanged[NUM_RADIOS];
    bool radioChanged;
    void saveSettings();
    void readSettings();
    void fillRadioModelInfo();
    void fillPortsInfo();
    void fillSpeedInfo();
    void fillDataBitsInfo();
    void fillStopBitsInfo();
    void fillParityInfo();
    void fillHandShakeInfo();
    void clearAvailRadio();
    void clearCurrentRadio();
    void enableCIVbox();





};

#endif // SETUPDIALOG_H
