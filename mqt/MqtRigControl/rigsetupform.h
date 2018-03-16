#ifndef RIGSETUPFORM_H
#define RIGSETUPFORM_H

#include <QWidget>
#include "rigcontrol.h"
#include "transvertsetupform.h"
#include "ui_rigsetupform.h"

namespace Ui {
    class rigSetupForm;
}


class RigSetupForm : public QWidget
{
    Q_OBJECT

public:
    explicit RigSetupForm(RigControl* _radio, scatParams* _radioData, QWidget *parent = nullptr);
    ~RigSetupForm();


    bool radioValueChanged;
    bool radioNameChanged;

    scatParams* getRadioData();



    QVector<TransVertSetupForm*> transVertTab;
    //QStringList availTransVerters;
    //int numAvailTransVerters;

    QString getRadioModel();
    void setRadioModel(QString m);

    void civSetToolTip();
    QString getCIVAddress();
    void setCIVAddress(QString v);
    void enableCIVEdit(bool enable);

    QString getComport();
    void setComport(QString p);

    QString getDataSpeed();
    void setDataSpeed(QString d);

    int comportAvial(QString comport);

    QString getDataBits();
    void setDataBits(QString d);

    QString getStopBits();
    void setStopBits(QString stop);

    QString getParityBits();
    void setParityBits(int b);

    QString getHandshake();
    void setHandshake(int h);

    QString getNetAddress();
    void setNetAddress(QString netAdd);

    QString getNetPortNum();
    void setNetPortNum(QString p);

    QString getMgmMode();
    void setMgmMode(QString p);

    QString getPollInterval();
    void setPollInterval(QString i);
    void pollIntervalVisible(bool s);

    void networkDataEntryVisible(bool v);
    void serialDataEntryVisible(bool v);


    bool getTransVertSelected();
    void setTransVertSelected(bool flag);

    void CIVEditVisible(bool visible);
    void transVertTabEnable(bool visible);

signals:
    void transVertTabChanged();


private slots:


    void radioModelSelected();
    void comportSelected();
    void comSpeedSelected();
    void comDataBitsSelected();
    void comStopBitsSelected();
    void comParitySelected();
    void comHandShakeSelected();
    void networkAddressSelected();
    void networkPortSelected();
    void pollIntervalSelected();
    void mgmModeSelected();


    void addTransVerter();
    void removeTransVerter();
    void renameTransVerter();
    void enableTransVertSelected(bool);
private:


    Ui::rigSetupForm *ui;
    RigControl *radio;
    scatParams *radioData;

    //TransVertSetupForm *transVerter;





    void fillHandShakeInfo();
    void fillParityInfo();
    void fillStopBitsInfo();
    void fillDataBitsInfo();
    void fillSpeedInfo();
    void fillPortsInfo();
    void fillRadioModelInfo();
    void fillMgmModes();


    void fillPollInterValInfo();
    void addTransVertTab(int tabNum, QString tabName);
    bool checkTransVerterNameMatch(QString transVertName);
};

#endif // RIGSETUPFORM_H
