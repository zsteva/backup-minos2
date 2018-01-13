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
#include <QVector>

#include "rotcontrol.h"
#include "setupdialog.h"
#include "rotsetupform.h"
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



    QString currentAntennaName;
    srotParams* currentAntenna;
    int currentAntIndex;

    QVector<srotParams*> availAntData;
    QStringList availAntennas;
    int numAvailAntennas;



    srotParams getCurrentAntenna() const;
    int getRotatorId(QString rotator);


    void readCurrentAntenna();
    void saveCurrentAntenna();
    QString getRotatorComPort(QString);

    void readSettings();
    void copyAntennaToCurrent(int antennaNumber);

    void setAppName(QString name);
    int comportAvial(int antennaNum, QString comport);
signals:

    void currentAntennaSettingChanged(QString);
    void antennaNameChange();


private slots:

    void saveButtonPushed();
    void cancelButtonPushed();


private:
    Ui::SetupDialog *ui;

    RotControl *rotator;

    QVector<rotSetupForm*> antennaTab;



    bool chkloadflg = false;
    QString appName = "";

    void saveSettings();

    void clearAvailRotators();
    void clearCurrentRotator();



    void clearAntennaValueChanged();
    void clearAntennaNameChanged();
    void serialDataEntryVisible(int antNumber, bool visible);
    void networkDataEntryVisible(int antNumber, bool visible);


    void getAvailAntennas();
    void addTab(int tabNum, QString tabName);
};

#endif // SETUPDIALOG_H
