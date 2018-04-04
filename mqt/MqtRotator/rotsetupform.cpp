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




#include "rotsetupform.h"
#include "ui_rotsetupform.h"
#include <QDebug>
#include <QLineEdit>
#include <QCheckBox>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>


static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");



rotSetupForm::rotSetupForm(RotControl* _rotator, srotParams* _antennaData, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rotSetupForm),
    antennaValueChanged(false),
    antennaNameChanged(false)
    //antennaChanged(false)
{
    ui->setupUi(this);
    ui->netAddressBox->setVisible(false);
    ui->netAddressLbl->setVisible(false);
    ui->netPortBox->setVisible(false);
    ui->portLbl->setVisible(false);

    rotator = _rotator;
    antennaData = _antennaData;

    fillRotatorModelInfo();
    fillPortsInfo();
    fillPollInterValInfo();
    fillSpeedInfo();
    fillDataBitsInfo();
    fillStopBitsInfo();
    fillParityBitsInfo();
    fillHandShakeInfo();


    networkDataEntryVisible(false);

    //connect(ui, SIGNAL(editingFinished()), this, SLOT(antennaNameFinished()));
    connect(ui->rotatorModelBox, SIGNAL(activated(int)), this, SLOT(rotatorModelSelected()));
    connect(ui->comPortBox, SIGNAL(activated(int)), this, SLOT(comportSelected()));
    connect(ui->comSpeedBox, SIGNAL(activated(int)), this, SLOT(comDataSpeedSelected()));
    connect(ui->comDataBitsBox, SIGNAL(activated(int)), this, SLOT(comDataBitsSelected()));
    connect(ui->comStopBitsBox, SIGNAL(activated(int)), this, SLOT(comStopBitsSelected()));
    connect(ui->comParityBox, SIGNAL(activated(int)), this, SLOT(comParityBitsSelected()));
    connect(ui->comHandShakeBox, SIGNAL(activated(int)), this, SLOT(comHandshakeSelected()));
    connect(ui->netAddressBox, SIGNAL(editingFinished()), this, SLOT(comNetAddressSelected()));
    connect(ui->netPortBox, SIGNAL(editingFinished()), this, SLOT(comNetPortNumSelected()));
    connect(ui->pollInterval, SIGNAL(activated(int)), this, SLOT(pollIntervalSelected()));
    connect(ui->sStopOffBut, SIGNAL(clicked(bool)), this, SLOT(sStopOffButSelected()));
    connect(ui->rotInvertBut, SIGNAL(clicked(bool)), this, SLOT(rotInvertButSelected()));
    connect(ui->compassBut, SIGNAL(clicked(bool)), this, SLOT(compassButSelected()));
    connect(ui->chkOverrun, SIGNAL(stateChanged(int)), this, SLOT(overlapSelected()));
    connect(ui->antOffset, SIGNAL(editingFinished()), this, SLOT(antennaOffSetSelected()));
    connect(ui->simCW_CCWcmd, SIGNAL(clicked(bool)), this, SLOT(simCWCCWCmdSelected()));
}

rotSetupForm::~rotSetupForm()
{
    delete ui;
}



/***************************** Rotator Models *************************/



void rotSetupForm::rotatorModelSelected()
{
    int rm;
    bool ok;
    QString rotModelName;
    QString rotMfgName;

    if (ui->rotatorModelBox->currentText() != antennaData->rotatorModel)
    {

        if (ui->rotatorModelBox->currentText() != "")
        {
            QString s = ui->rotatorModelBox->currentText();
            antennaData->rotatorModel = s;
            s = s.trimmed();
            QStringList antdetails = s.split(',');
            s = antdetails[0];
            antennaData->rotatorModelNumber = s.toInt(&ok, 10);
            s = antdetails[1];
            antennaData->rotatorManufacturer = s.trimmed();
            s = antdetails[2];
            antennaData->rotatorModelName = s.trimmed();


            antennaData->rotatorModel = ui->rotatorModelBox->currentText();
            if (rotator->getModelInfo(antennaData->rotatorModel, &rm, &rotMfgName, &rotModelName) == -1)
            {
                // error
                antennaData->rotatorModelNumber = 0;
                antennaData->rotatorModelName = "";
                antennaData->rotatorManufacturer = "";
            }
            else
            {
                antennaData->rotatorModelNumber = rm;
                antennaData->rotatorModelName = rotModelName;
                antennaData->rotatorManufacturer = rotMfgName;

            }

            pollIntervalVisible(true);
            antennaOffSetVisible(true);


            if (setEndStopType(antennaData))
            {

                // set ui according to rotator

                if (antennaData->endStopType == ROT_0_360)
                {

                    ui->chkOverrun->setVisible(false);
                    ui->chkOverrun->setChecked(false);
                    sStopButtonsVisible(true);
                    setSStopOffButChecked(true);



                }
                else if (antennaData->endStopType == ROT_0_450)
                {

                    ui->chkOverrun->setVisible(true);
                    ui->chkOverrun->setChecked(true);
                    sStopButtonsVisible(false);
                    setSStopOffButChecked(true);

                }
                else
                {
                    ui->chkOverrun->setVisible(false);
                    ui->chkOverrun->setChecked(false);
                    sStopButtonsVisible(false);
                    setSStopOffButChecked(true);

                }
            }


            if (antennaData->portType == RIG_PORT_NETWORK || antennaData->portType == RIG_PORT_UDP_NETWORK)
            {
               serialDataEntryVisible(false);
               networkDataEntryVisible(true);
            }
            else if (antennaData->portType == RIG_PORT_SERIAL)
            {
                serialDataEntryVisible(true);
                networkDataEntryVisible(false);
            }
            else if (antennaData->portType == RIG_PORT_NONE)
            {
                serialDataEntryVisible(false);
                networkDataEntryVisible(false);
            }

            if (antennaData->supportCwCcwCmd)
            {
                setSimCW_CCWcmdVisible(false);
                //setSimCW_CCWcmdChecked(false);
                antennaData->simCwCcwCmd = false;
            }
            else
            {
                setSimCW_CCWcmdVisible(true);
                setSimCW_CCWcmdChecked(true);
                antennaData->simCwCcwCmd = true;
            }



        }
        else
        {
            // no rotator model selected
            //antennaData->rotatorModelNumber = 0;
            //antennaData->rotatorModelName = "";
            //antennaData->rotatorManufacturer = "";
            networkDataEntryVisible(false);
            setOverRunFlagVisible(false);
            sStopButtonsVisible(false);
            pollIntervalVisible(false);
            antennaOffSetVisible(false);
            serialDataEntryVisible(false);
            setSimCW_CCWcmdVisible(false);

        }




        antennaValueChanged = true;


    }
}


bool rotSetupForm::setEndStopType(srotParams* antennaData)
{

    int minRot = 0.0;
    int maxRot = 0.0;


    if (getMaxMinRotationData(antennaData->rotatorModelNumber, &maxRot, &minRot) >= 0)
    {
        //antennaData->max_azimuth = maxRot;
        //antennaData->min_azimuth = minRot;
        antennaData->rotatorCWEndStop = maxRot;
        antennaData->rotatorCCWEndStop = minRot;

        // define type of rotator

        if (maxRot == 180 && minRot == -180)
        {
            antennaData->max_azimuth = maxRot;
            antennaData->min_azimuth = minRot;
            antennaData->rotType = ROT_NEG180_180;
            antennaData->endStopType = ROT_NEG180_180;
            antennaData->overRunFlag = false;
            antennaData->southStopType = S_STOPOFF;
        }

        else if (maxRot == COMPASS_MAX360 && minRot == COMPASS_MIN0)
        {
            antennaData->max_azimuth = maxRot;
            antennaData->min_azimuth = minRot;
            antennaData->rotType = ROT_0_360;
            antennaData->endStopType = ROT_0_360;

            antennaData->overRunFlag = false;
            antennaData->southStopType = S_STOPOFF;
        }
        else if (maxRot > COMPASS_MAX360 && minRot == COMPASS_MIN0 )
        {

            antennaData->rotType = ROT_0_450;
            antennaData->endStopType = ROT_0_450;
            antennaData->max_azimuth = maxRot;
            antennaData->min_azimuth = minRot;
            antennaData->overRunFlag = true;
            antennaData->southStopType = S_STOPOFF;

         }
        else if (maxRot > COMPASS_MAX360 && minRot < COMPASS_MIN0 )
        {

            antennaData->max_azimuth = maxRot;
            antennaData->min_azimuth = minRot;
            antennaData->rotType = ROT_NEG180_540;
            antennaData->endStopType = ROT_NEG180_540;
            antennaData->overRunFlag = true;
            antennaData->southStopType = S_STOPOFF;
        }

        // flag if rotator supports CW and CCW commands
        antennaData->supportCwCcwCmd = getCwCcwCmdFlag(antennaData->rotatorModelNumber);

        rig_port_e portType = RIG_PORT_NONE;

        if (rotator->getPortType(antennaData->rotatorModelNumber, &portType) != -1)
        {
            antennaData->portType = portType;
        }
        else
        {
            return false; // error
        }


    }
    else
    {
        // no rotator model selected
        antennaData->rotatorModelNumber = 0;
        antennaData->rotatorModelName = "";
        antennaData->rotatorManufacturer = "";
        return false;
    }

    return true;

}



QString rotSetupForm::getRotatorModel()
{
    return ui->rotatorModelBox->currentText();
}


void rotSetupForm::setRotatorModel(QString m)
{
    ui->rotatorModelBox->setCurrentIndex(ui->rotatorModelBox->findText(m));

}

/***************************** Comports *************************/



void rotSetupForm::comportSelected()
{
    if (ui->comPortBox->currentText() != antennaData->comport)
    {
        antennaData->comport = ui->comPortBox->currentText();
        antennaValueChanged = true;

    }
}

QString rotSetupForm::getComport()
{
    return ui->comPortBox->currentText();
}

void rotSetupForm::setComport(QString p)
{
    ui->comPortBox->setCurrentIndex(ui->comPortBox->findText(p));
}

/***************************** Data Speed *************************/

void rotSetupForm::comDataSpeedSelected()
{
    if (antennaData->baudrate != ui->comSpeedBox->currentText().toInt())
    {
        antennaData->baudrate = ui->comSpeedBox->currentText().toInt();
        antennaValueChanged = true;

    }
}

QString rotSetupForm::getDataSpeed()
{
    return ui->comSpeedBox->currentText();
}

void rotSetupForm::setDataSpeed(QString d)
{
    ui->comSpeedBox->setCurrentIndex(ui->comSpeedBox->findText(d));
}

/***************************** Data Bits *************************/



void rotSetupForm::comDataBitsSelected()
{
    antennaData->databits = ui->comDataBitsBox->currentText().toInt();
    antennaValueChanged = true;

}

QString rotSetupForm::getDataBits()
{
    return ui->comDataBitsBox->currentText();
}

void rotSetupForm::setDataBits(QString d)
{
    ui->comDataBitsBox->setCurrentIndex(ui->comDataBitsBox->findText(d));
}

/***************************** Stop Bits *************************/

void rotSetupForm::comStopBitsSelected()
{
    if (antennaData->stopbits != ui->comStopBitsBox->currentText().toInt())
    {
        antennaData->stopbits = ui->comStopBitsBox->currentText().toInt();
        antennaValueChanged = true;

    }
}

QString rotSetupForm::getStopBits()
{
    return ui->comStopBitsBox->currentText();
}

void rotSetupForm::setStopBits(QString stop)
{
    ui->comStopBitsBox->setCurrentIndex(ui->comStopBitsBox->findText(stop));
}

/***************************** Parity Bits *************************/

void rotSetupForm::comParityBitsSelected()
{
    if (antennaData->parity != rotator->getSerialParityCode(ui->comParityBox->currentIndex()))
    {
        antennaData->parity = rotator->getSerialParityCode(ui->comParityBox->currentIndex());
        antennaValueChanged = true;

    }
}

QString rotSetupForm::getParityBits()
{
    return ui->comParityBox->currentText();
}

void rotSetupForm::setParityBits(int b)
{
    ui->comParityBox->setCurrentIndex(b);
}

/***************************** Handshake *************************/

void rotSetupForm::comHandshakeSelected()
{
    if (antennaData->handshake != rotator->getSerialHandshakeCode(ui->comHandShakeBox->currentIndex()))
    {
        antennaData->handshake = rotator->getSerialHandshakeCode(ui->comHandShakeBox->currentIndex());
        antennaValueChanged = true;

    }
}

QString rotSetupForm::getHandshake()
{
    return ui->comHandShakeBox->currentText();
}

void rotSetupForm::setHandshake(int h)
{
    ui->comHandShakeBox->setCurrentIndex(h);
}


/***************************** Network Address *************************/

void rotSetupForm::comNetAddressSelected()
{
    if (antennaData->networkAdd != ui->netAddressBox->text())
    {
        antennaData->networkAdd = ui->netAddressBox->text();
        antennaValueChanged = true;

    }

}

QString rotSetupForm::getNetAddress()
{
    return ui->netAddressBox->text();
}

void rotSetupForm::setNetAddress(QString netAdd)
{
    ui->netAddressBox->setText(netAdd);
}
/***************************** Network Port *************************/

void rotSetupForm::comNetPortNumSelected()
{
    if (antennaData->networkPort != ui->netPortBox->text())
    {
        antennaData->networkPort = ui->netPortBox->text();
        antennaValueChanged = true;

    }

}

QString rotSetupForm::getNetPortNum()
{
    return ui->netPortBox->text();
}

void rotSetupForm::setNetPortNum(QString p)
{
    ui->netPortBox->setText(p);
}

/***************************** Poll Interval *************************/

void rotSetupForm::pollIntervalSelected()
{
    if (antennaData->pollInterval != ui->pollInterval->currentText())
    {
        antennaData->pollInterval = ui->pollInterval->currentText();
        antennaValueChanged = true;

    }
}

QString rotSetupForm:: getPollInterval()
{
    return ui->pollInterval->currentText();
}

void rotSetupForm::setPollInterval(QString i)
{
    ui->pollInterval->setCurrentIndex(ui->pollInterval->findText(i));
}

void rotSetupForm::pollIntervalVisible(bool s)
{
    ui->pollInterval->setVisible(s);
    ui->intervallbl->setVisible(s);
}

/***************************** South Stop Off Selected *************************/

void rotSetupForm::sStopOffButSelected()
{
//    if (!chkloadflg)
//    {

    antennaData->southStopType = S_STOPOFF;
    antennaData->max_azimuth = COMPASS_MAX360;
    antennaData->min_azimuth = COMPASS_MIN0;
    antennaData->endStopType = ROT_0_360;

    antennaValueChanged = true;

//    }
}


bool rotSetupForm::getsStopOffBut()
{
    return ui->sStopOffBut->isChecked();
}

void rotSetupForm::setSStopOffButChecked(bool s)
{
    ui->sStopOffBut->setChecked(s);
}

void rotSetupForm::setSStopOffButVisible(bool s)
{
    ui->sStopOffBut->setVisible(s);

}


void rotSetupForm::sStopButtonsVisible(bool visible)
{
    ui->sStopFrame->setVisible(visible);
    setCompassButVisible(visible);
    setRotInvertButVisible(visible);
    setCompassButVisible(visible);

}

void rotSetupForm::setSStopButtons(southStop stopType)
{
    if (stopType == S_STOPOFF)
    {
        setRotInvertButChecked(true);
    }
    else if (stopType == S_STOPINV)
    {
        setRotInvertButVisible(true);
    }
    else if (stopType == S_STOPCOMP)
    {
        setCompassButChecked(true);
    }

}


/***************************** rotator inverted button Selected *************************/

void rotSetupForm::rotInvertButSelected()
{
//    if (!chkloadflg)
//    {

    antennaData->southStopType = S_STOPINV;
    antennaData->max_azimuth = COMPASS_MAX360;
    antennaData->min_azimuth = COMPASS_MIN0;
    antennaData->endStopType = ROT_0_360;

    antennaValueChanged = true;


//    }
}

bool rotSetupForm::getRotInvertBut()
{
    return ui->rotInvertBut->isChecked();
}

void rotSetupForm::setRotInvertButChecked(bool s)
{
    ui->rotInvertBut->setChecked(s);
}

void rotSetupForm::setRotInvertButVisible(bool s)
{
    ui->rotInvertBut->setVisible(s);

}
/***************************** compass button Selected *************************/

void rotSetupForm::compassButSelected()
{
//    if (!chkloadflg)
//    {

    antennaData->max_azimuth = COMPASS_HALF - 1;
    antennaData->min_azimuth = COMPASS_HALF + 1;
    antennaData->endStopType = ROT_180_180;

    antennaValueChanged = true;

//    }
}



bool rotSetupForm::getCompassBut()
{
    return ui->compassBut->isChecked();
}

void rotSetupForm::setCompassButChecked(bool s)
{
    ui->compassBut->setChecked(s);
}

void rotSetupForm::setCompassButVisible(bool s)
{
    ui->compassBut->setVisible(s);

}

/***************************** Check Overrun *************************/

void rotSetupForm::overlapSelected()
{
//    if (!chkloadflg)
//    {

        antennaData->overRunFlag = ui->chkOverrun->isChecked();

        if (antennaData->overRunFlag && (antennaData->endStopType == ROT_0_360 || antennaData->endStopType == ROT_180_180))
        {
            // restore to ROT_0_450 type
            antennaData->max_azimuth = antennaData->rotatorCWEndStop;
            antennaData->min_azimuth = antennaData->rotatorCCWEndStop;
            antennaData->endStopType = antennaData->rotType;
            sStopButtonsVisible(false);
            setSStopOffButChecked(true);
            antennaData->southStopType = S_STOPOFF;

        }
        else if (!antennaData->overRunFlag && antennaData->endStopType == ROT_0_450)
        {
            antennaData->max_azimuth = COMPASS_MAX360;
            antennaData->min_azimuth = COMPASS_MIN0;
            antennaData->endStopType = ROT_0_360;
            sStopButtonsVisible(true);
            setSStopOffButChecked(true);
            antennaData->southStopType = S_STOPOFF;

        }




        antennaValueChanged = true;



//    }


}


bool rotSetupForm::getCheckOverrun()
{
    return ui->chkOverrun->checkState();
}

void rotSetupForm::setCheckOverrun(bool s)
{
    ui->chkOverrun->setChecked(s);
}

void rotSetupForm::setOverRunFlagVisible(bool s)
{
    ui->chkOverrun->setVisible(s);

}


/*************************** Simulate CW CCW Command Check *************/


void rotSetupForm::simCWCCWCmdSelected()
{
    if (ui->simCW_CCWcmd->isChecked() != antennaData->simCwCcwCmd)
    {
        antennaData->simCwCcwCmd = ui->simCW_CCWcmd->isChecked();
    }
    antennaValueChanged = true;
}

void rotSetupForm::setSimCW_CCWcmdVisible(bool visible)
{
    ui->simCW_CCWcmd->setVisible(visible);
}

void rotSetupForm::setSimCW_CCWcmdChecked(bool checked)
{
    ui->simCW_CCWcmd->setChecked(checked);
}



/***************************** Antenna Offset *************************/

void rotSetupForm::antennaOffSetSelected()
{
    QMessageBox messageBox;

    if (ui->antOffset->text() != "")
    {
        int offset = ui->antOffset->text().toInt();
        if (offset < minOffset || offset > maxOffset)
        {
            ui->antOffset->setText("0");
            QString msg = "Value must be between " + QString::number(minOffset) + " and " + QString::number(maxOffset);
            messageBox.critical(this, "Antenna Offset Error", msg);

        }
        else
        {

            if (offset  != antennaData->antennaOffset)
            {
                antennaData->antennaOffset = offset;
                antennaValueChanged = true;

            }

        }


    }
}

QString rotSetupForm::getAntennaOffset()
{
    return ui->antOffset->text();
}

void rotSetupForm::setAntennaOffset(QString o)
{
    ui->antOffset->setText(o);
}

void rotSetupForm::antennaOffSetVisible(bool s)
{
    ui->offSetLbl->setVisible(s);
    ui->antOffset->setVisible(s);
}

/*************************** Serial Data Entry Visible ***************/

void rotSetupForm::serialDataEntryVisible(bool v)
{
    ui->comPortBox->setVisible(v);
    ui->comportLbl->setVisible(v);
    ui->comSpeedBox->setVisible(v);
    ui->speedLbl->setVisible(v);
    ui->comDataBitsBox->setVisible(v);
    ui->dBitsLbl->setVisible(v);
    ui->comStopBitsBox->setVisible(v);
    ui->stopLbl->setVisible(v);
    ui->comParityBox->setVisible(v);
    ui->parityLbl->setVisible(v);
    ui->comHandShakeBox->setVisible(v);
    ui->handLbl->setVisible(v);
}

/*************************** Network Data Entry Visible ***************/

void rotSetupForm::networkDataEntryVisible(bool v)
{
    ui->netAddressBox->setVisible(v);
    ui->netAddressLbl->setVisible(v);
    ui->netPortBox->setVisible(v);
    ui->portLbl->setVisible(v);
}





void rotSetupForm::fillRotatorModelInfo()
{
    ui->rotatorModelBox->clear();
    rotator->getRotatorList(ui->rotatorModelBox);
}


void rotSetupForm::fillPollInterValInfo()
{
    QStringList pollTimeStr;
    pollTimeStr << "0.5" << "1" << "2" << "3";

    ui->pollInterval->clear();
    ui->pollInterval->addItems(pollTimeStr);
}



void rotSetupForm::fillPortsInfo()
{
    ui->comPortBox->clear();


    QString description;
    QString manufacturer;
    QString serialNumber;

    ui->comPortBox->addItem("");

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);


        ui->comPortBox->addItem(list.first(), list);
    }
}




void rotSetupForm::fillSpeedInfo()
{

    QStringList baudrateStr = rotator->getBaudRateNames();

    ui->comSpeedBox->clear();
    ui->comSpeedBox->addItems(baudrateStr);
}

void rotSetupForm::fillDataBitsInfo()
{
    QStringList databitsStr = rotator->getDataBitsNames();

    ui->comDataBitsBox->clear();
    ui->comDataBitsBox->addItems(databitsStr);
}

void rotSetupForm::fillStopBitsInfo()
{
    QStringList stopbitsStr = rotator->getStopBitsNames();

    ui->comStopBitsBox->clear();
    ui->comStopBitsBox->addItems(stopbitsStr);
}



void rotSetupForm::fillParityBitsInfo()
{

    QStringList parityStr = rotator->getParityCodeNames();

    ui->comParityBox->clear();
    ui->comParityBox->addItems(parityStr);

}



void rotSetupForm::fillHandShakeInfo()
{

    QStringList handshakeStr = rotator->getHandShakeNames();

    ui->comHandShakeBox->clear();
    ui->comHandShakeBox->addItems(handshakeStr);

}

// returns -1 if no text found
int rotSetupForm::comportAvial(QString comport)
{
    return ui->comPortBox->findText(comport);
}



int rotSetupForm::getMaxMinRotationData(int rotatorNumber, int *maxRot, int *minRot)
{

    int retCode = 0;
    retCode = rotator->getMaxMinRotation(rotatorNumber, maxRot, minRot);

    return retCode;

}

bool rotSetupForm::getCwCcwCmdFlag(int rotatorNumber)
{
    int retCode = 0;
    bool value = false;
    retCode = rotator->getSupportCwCcwCmd(rotatorNumber, &value);
    if (retCode < 0)
    {
        //trace(QString("Error getting CwCcwSupport flag for rotator number %1").arg(QString::number(rotatorNumber)));
    }

    return value;

}
