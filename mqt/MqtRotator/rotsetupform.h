/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rig Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2018
//
//
//
//
/////////////////////////////////////////////////////////////////////////////



#ifndef ROTSETUPFORM_H
#define ROTSETUPFORM_H

#include <QWidget>
#include "rotcontrol.h"

namespace Ui {
class rotSetupForm;
}

class rotSetupForm : public QWidget
{
    Q_OBJECT

public:
    explicit rotSetupForm(RotControl* rotator, srotParams* _antennaData, QWidget *parent = 0);
    ~rotSetupForm();

    srotParams *antennaData;
    bool antennaValueChanged;
    bool antennaNameChanged;




    QString getAntennaName();
    void setAntennaName(QString n);

    QString getRotatorModel();
    void setRotatorModel(QString m);


    QString getComport();
    void setComport(QString p);

    QString getDataSpeed();
    void setDataSpeed(QString d);

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

    QString getPollInterval();
    void setPollInterval(QString i);


    bool getCheckStop();
    void setCheckStop(bool s);
    void setCheckStopVisible(bool s);

    bool getCheckOverrun();
    void setCheckOverrun(bool s);

    QString getAntennaOffset();
    void setAntennaOffset(QString o);
    void setOverRunFlagVisible(bool s);

    void serialDataEntryVisible(bool v);
    void networkDataEntryVisible(bool v);

    int comportAvial(QString comport);
    int getMaxMinRotationData(int rotatorNumber, int *maxRot, int *minRot);


    void antennaOffSetVisible(bool s);
    void pollIntervalVisible(bool s);
    bool setEndStopType(srotParams *antennaData);
    void setSimCW_CCWcmdVisible(bool visible);
    void setSimCW_CCWcmdChecked(bool checked);
private slots:
//    void antennaNameFinished();
    void rotatorModelSelected();
    void comportSelected();
    void comDataSpeedSelected();
    void comDataBitsSelected();
    void comStopBitsSelected();
    void comParityBitsSelected();
    void comHandshakeSelected();
    void comNetAddressSelected();
    void comNetPortNumSelected();
    void pollIntervalSelected();
    void southStopSelected();
    void overlapSelected();
    void antennaOffSetSelected();
    void simCWCCWCmdSelected();
private:
    Ui::rotSetupForm *ui;

    RotControl *rotator;

    const int minOffset = -90;
    const int maxOffset = 90;

    void loadComports();
    void loadRotatorModels();
    void loadAntennaName();
    void fillRotatorModelInfo();
    void fillPollInterValInfo();
    void fillPortsInfo();
    void fillSpeedInfo();
    void fillDataBitsInfo();
    void fillStopBitsInfo();
    void fillParityBitsInfo();
    void fillHandShakeInfo();


    bool getCwCcwCmdFlag(int rotatorNumber);

};

#endif // ROTSETUPFORM_H
