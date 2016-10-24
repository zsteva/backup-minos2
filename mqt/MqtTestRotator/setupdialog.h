#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include "rotcontrol.h"

#define NUM_ROTATOR_PROTOCOLS 2
#define NUM_ANTENNAS 5


class QIntValidator;
class QComboBox;
class QLineEdit;



namespace Ui {
class SetupDialog;
}

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDialog(RotControl *rotator, QWidget *parent = 0);
    ~SetupDialog();


//    struct RotatorType{
//            QString protocol;
//            int id;
//    };



//    struct Rotators {
//        QString name;
//        RotatorType rotator;
//        int rotator_idx;
//        QString comPort;
//        int comPort_idx;
//        qint32 baudRate;
//        QString stringBaudRate;
//        QSerialPort::DataBits dataBits;
//        QString stringDataBits;
//        QSerialPort::Parity parity;
//        QString stringParity;
//        QSerialPort::StopBits stopBits;
//        QString stringStopBits;
//        QSerialPort::FlowControl flowControl;
//        QString stringFlowControl;
//    };





    srotParams getCurrentAntenna() const;
    int getRotatorId(QString rotator);


    srotParams availAntennas[NUM_ANTENNAS];
    srotParams currentAntenna;

//    RotatorType *rotProtocol[NUM_ROTATOR_PROTOCOLS];

    QString getRotatorProtocol(QString );
    void readCurrentAntenna();
    QString getRotatorComPort(QString);
    void saveCurrentAntenna();

protected slots:

    void antennaNameFinished(int);
    void rotatorModelSelected(int);
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
    QComboBox *comPorts[NUM_ANTENNAS];
    QComboBox *comSpeed[NUM_ANTENNAS];
    QComboBox *comDataBits[NUM_ANTENNAS];
    QComboBox *comStopBits[NUM_ANTENNAS];
    QComboBox *comParity[NUM_ANTENNAS];
    QComboBox *comHandShake[NUM_ANTENNAS];
    QIntValidator *intValidator;
//    RotatorType *yaesu;
//    RotatorType *prositel;
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
