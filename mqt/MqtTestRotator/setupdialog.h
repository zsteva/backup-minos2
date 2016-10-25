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
    QStringList parityStr = (QStringList() << "None" << "Odd" << "Even" << "Mark" << "Space");
    QStringList handshakeStr = (QStringList() << "None" << "XON/XOFF" << "CTS/RTS");
    QStringList baudrateStr = (QStringList() << "" << "1200" << "2400" << "4800" << "9600" << "19200" << "38400");
    QStringList databitsStr = (QStringList() << "" << "7" << "8" );
    QStringList stopbitsStr = (QStringList()<< "" << "1" << "2");
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
