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
#include <QSettings>
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

    QVector<srotParams*> availAntData;
    QStringList availAntennas;
    int numAvailAntennas;



    QString getCurrentAntenna() const;
    int getRotatorId(QString rotator);


    void readCurrentAntenna();
    void saveCurrentAntenna();
    QString getRotatorComPort(QString);

    void readSettings();
    void copyAntennaToCurrent(int antennaNumber);

    void setAppName(QString name);
    int comportAvial(QString comport);
    int findCurrentAntenna(QString currentAntName);
    void setTabToCurrentAntenna();
signals:

    void currentAntennaSettingChanged(QString);
    void antennaNameChange();
    void antennaTabChanged();


private slots:

    void saveButtonPushed();
    void cancelButtonPushed();

    void addAntenna();
    void removeAntenna();
    void editAntennaName();
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
    bool checkAntNameMatch(QString antName);
    void saveAntenna(int i);


    void loadSettingsToTab(int tabNum);
};

#endif // SETUPDIALOG_H
