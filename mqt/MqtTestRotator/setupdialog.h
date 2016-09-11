#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>

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
    explicit SetupDialog(QWidget *parent = 0);
    ~SetupDialog();


    struct RotatorType{
            QString protocol;
            int id;
    };


    struct Rotators {
        QString name;
        RotatorType rotator;
        int rotator_idx;
        QString comPort;
        int comPort_idx;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
    };





    SetupDialog::Rotators getCurrentAntenna() const;
    int getRotatorId(QString rotator);


    Rotators availAntennas[NUM_ANTENNAS];
    Rotators currentAntenna;
    RotatorType *rotProtocol[NUM_ROTATOR_PROTOCOLS];

    QString getRotatorProtocol(QString );
    void readCurrentAntenna();
    QString getRotatorComPort(QString);
    void saveCurrentAntenna();

protected slots:

    void comportSelected(int);
    void protocolSelected(int);
    void rotatorNameFinished(int);


    void saveButtonPushed();
    void cancelButtonPushed();
private:
    Ui::SetupDialog *ui;

    QComboBox *comPorts[NUM_ANTENNAS];
    QComboBox *protocols[NUM_ANTENNAS];
    QLineEdit *antennaName[NUM_ANTENNAS];
    QIntValidator *intValidator;
    RotatorType *yaesu;
    RotatorType *prositel;
    bool antennaValueChanged[NUM_ANTENNAS];
    bool antennaChanged;
    void saveSettings();
    void readSettings();
    void fillPortsInfo();
    void fillProtocolInfo();
    void clearAvailRotators();
    void clearCurrentRotator();
    void initRotProtocols();




};

#endif // SETUPDIALOG_H
