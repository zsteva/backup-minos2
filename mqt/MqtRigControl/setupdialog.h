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
#include <QVector>
#include "rigcontrol.h"
#include "rigsetupform.h"
#include "transvertsetupform.h"





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
    explicit SetupDialog(RigControl* rig, const QVector<BandDetail*> _bands, QWidget *parent = nullptr);
    ~SetupDialog();


    QString currentRadioName;
    scatParams currentRadio;

    QVector<scatParams*> availRadioData;
    QStringList availRadios;
    int numAvailRadios;

    QVector<BandDetail*> bands;

    QStringList addedRadioTabs;     // tracked edited data
    QStringList removeRadioTabs;
    QStringList renameRadioTabs;   // old radio names


    int getRadioId(QString rotator);


    void readCurrentRadio();
    QString getRadioComPort(QString);
    void saveCurrentRadio();

    void copyRadioToCurrent(int radioNumber);

    void readSettings();
    void setAppName(QString name);
    int comportAvial(int radioNum, QString comport);
    int findCurrentRadio(QString currentRadioName);
    void setTabToCurrentRadio();



signals:

    void radioNameChange();
    void currentRadioSettingChanged(QString);
    void radioTabChanged();
    void transVertNameChanged();

private slots:

    void saveButtonPushed();
    void cancelButtonPushed();

    void addRadio();
    void removeRadio();
    void editRadioName();
private:
    Ui::SetupDialog *ui;

    RigControl *radio;

    QVector<RigSetupForm*> radioTab;

    QSettings availRadioIni;
    //QSettings transVerterIni;


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
    void loadMgmModes();
    void saveMgmList();
    void addTab(int tabNum, QString tabName);
    void loadSettingsToTab(int tabNum);
    void saveRadio(int i);
    bool checkRadioNameMatch(QString radioName);
    void saveTranVerterSetting(int radioNum, int transVertNum, QSettings  &config);
    void readTranVerterSetting(int radioNum, int transVertNum, QSettings &config);
    void loadBands();
    void getRadioSetting(int radNum, QSettings &config);
    void saveRadioData(int radNum, QSettings &config);

    void initSetup();
};

#endif // SETUPDIALOG_H
