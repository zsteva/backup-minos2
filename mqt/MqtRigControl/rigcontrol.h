/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rig Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2017
//
//
//
// Hamlib Library
//
/////////////////////////////////////////////////////////////////////////////


#ifndef RIGCONTROL_H
#define RIGCONTROL_H

#include <QObject>
#include <QComboBox>
#include <QStringList>
#include "base_pch.h"
#include <hamlib/rig.h>


#define MAXCONFLEN 128

extern "C" int write_block(hamlib_port_t *p, const char *txbuffer, size_t count);
extern "C" int read_block(hamlib_port_t *p, char *rxbuffer, size_t count);

bool model_Sort(const rig_caps *caps1,const rig_caps *caps2);
int rig_message_cb(enum rig_debug_level_e, rig_ptr_t, const char*, va_list);

namespace serialData
{
const serial_parity_e parityCodes[] = {RIG_PARITY_NONE, RIG_PARITY_ODD, RIG_PARITY_EVEN, RIG_PARITY_MARK, RIG_PARITY_SPACE};
const serial_handshake_e handshakeCodes[] = { RIG_HANDSHAKE_NONE, RIG_HANDSHAKE_XONXOFF, RIG_HANDSHAKE_HARDWARE };
const QStringList parityStr = (QStringList() << "None" << "Odd" << "Even" << "Mark" << "Space");
const QStringList handshakeStr = (QStringList() << "None" << "XON/XOFF" << "CTS/RTS");
const QStringList baudrateStr = (QStringList() << "" << "1200" << "2400" << "4800" << "9600" << "19200" << "38400");
const QStringList databitsStr = (QStringList() << "" << "7" << "8" );
const QStringList stopbitsStr = (QStringList()<< "" << "1" << "2");


const QStringList hamlibErrorMsg = {"No Error, operation completed sucessfully",
                                    "Invalid parameter",
                                    "Invalid configuration",
                                    "Memory shortage",
                                    "Function not implemented",
                                    "Communication timed out",
                                    "IO error, including open failed",
                                    "Internal Hamlib error",
                                    "Protocol error",
                                    "Command rejected by the rig",
                                    "Command performed, but arg truncated",
                                    "Function not available",
                                    "VFO not targetable",
                                    "Error talking on the bus",
                                    "Collision on the bus",
                                    "NULL RIG handle or any invalid pointer parameter in get arg",
                                    "Invalid VFO",
                                    "RIG_EDOM"};


}


class TransVertParams
{

public:

    QString transVertName;
    QString band;
    QString transVertOffsetStr;
    freq_t transVertOffset = 0.0;
    bool transVertNegative  = false;
    bool enableTransSwitch = false;
    QString transSwitchNum;

};


// This was the hamlib catParams structure, other fields have been added
// to support other functions.

struct scatParams
{

  QString radioName;
  QString radioNumber;
  QString comport; /**<  serial port device*/
  QString radioMfg_Name;
  QString radioModel;
  QString radioModelName;
  int radioModelNumber = 0;
  QString pollInterval = "1";
  QString civAddress;
  int baudrate = 0; /**<  serial port baudrate*/
  int parity = 0;
  int stopbits = 0;
  int databits = 0;
  int handshake = 0;
  int portType = int(RIG_PORT_NONE);
  QString networkAdd;
  QString networkPort;
  bool enableCAT = false;
  bool enableSerialPTT  = false;
  QString pttSerialPort;
  bool activeRTS  = false;
  bool activeDTR  = false;
  bool nactiveRTS  = false;
  bool nactiveDTR  = false;
  QString mgmMode;
  ptt_type_t pttType;
  bool transVertEnable  = false;
  QVector<TransVertParams*> transVertSettings;
  QStringList transVertNames;
  int numTransverters = 0;

};



//#define NUM_VFO 2
//#define VFOA 0
//#define VFOB 1
//#define CURVFO VFOA

class RigControl : public QObject
{
    Q_OBJECT
public:
    explicit RigControl(QObject *parent = 0);
    ~RigControl();
    int init(scatParams currentRadio);
    bool enabled() {return rigControlEnabled;}

    int getFrequency(vfo_t vfo, freq_t*);
    int setFrequency(freq_t frequency, vfo_t vfo);

    int getMode(vfo_t vfo, rmode_t *mode, pbwidth_t *width);
    int setMode(vfo_t vfo, rmode_t mode, pbwidth_t passBandwidth);
    QString convertModeQstr(rmode_t);

    int getVfo(vfo_t*);
    int setVfo(vfo_t);
    QString convertVfoQStr(vfo_t);

    int getModelNumber(int idx);
    int getRigModelIndex();
    void getRigList();
    bool getRigList(QComboBox *cb);
    const char * getMfg_Name(int idx);
    const char * getModel_Name(int idx);
    //void disable();
    int closeRig();
    scatParams* params() {return &catParams;}
    //void activatePTT(bool b);
    //double getTxDelay() {return catParams.txOnDelay;}
    //int rawCommand(QByteArray ba);
    //QString initError;
    serial_parity_e getSerialParityCode(int index);
    serial_handshake_e getSerialHandshakeCode(int index);
    QStringList getParityCodeNames();
    QStringList getHandShakeNames();
    QStringList getBaudRateNames();
    QStringList getDataBitsNames();
    QStringList getStopBitsNames();
    void set_serialConnected(bool connectFlag);
    bool get_serialConnected();
    QStringList gethamlibErrorMsg();
    QString gethamlibErrorMsg(int errorCode);
    QString gethamlibVersion();

    int rig_message_cb(enum rig_debug_level_e debug_level, const char *fmt, va_list ap);

    rmode_t convertQStrMode(QString mode);

    int passBandState;
    pbwidth_t passbandNarrow(rmode_t mode);
    pbwidth_t passbandNormal(rmode_t mode);
    pbwidth_t passbandWide(rmode_t mode);
    pbwidth_t lookUpPassBand(QString, int);
    void setPassBand(QString mode, int modeState);
    pbwidth_t getPassBand();
    void buildPassBandTable(QString);

    int rigConvertQStrMode(QString mode);
    int getPortType(int rigNumber, rig_port_e *portType);
    int getModelInfo(QString radioModel, int *radioModelNumber, QString *radioMfgName, QString *radioModelName);
    int getRit(vfo_t vfo, shortfreq_t *ritfreq);
    int setRit(vfo_t vfo, shortfreq_t ritfreq);
    int supportRit(int rigNumber, bool *ritFlag);
    bool checkFreqValid(freq_t freq, rmode_t mode);
signals:
    void frequency_updated(double);
    void debug_protocol(QString);

  private:
    hamlib_port_t myport;
    RIG *my_rig = 0;            // handle to rig instance
//    freq_t frequency;            // frequency
//    rmode_t rmode;          // radio mode of operation
    pbwidth_t pbwidth;

//    vfo_t vfo;              // vfo selection
//    int strength;           // S-Meter level
//    int retcode;            // generic return code from functions


    pbwidth_t cwPassbandNar = 500;
    pbwidth_t cwPassbandNor = 2200;
    pbwidth_t cwPassbandWide = 0;

    pbwidth_t usbPassbandNar = 0;
    pbwidth_t usbPassbandNor = 2200;
    pbwidth_t usbPassbandWide = 0;

    pbwidth_t fmPassbandNar = 0;
    pbwidth_t fmPassbandNor = 9000;
    pbwidth_t fmPassbandWide = 0;

    pbwidth_t mgmPassbandNar = 0;
    pbwidth_t mgmPassbandNor = 2200;
    pbwidth_t mgmPassbandWide = 0;

    pbwidth_t passBandWidth[4][3];




    rig_model_t myrig_model;
    bool rigControlEnabled;
    bool serialConnected;
    void errorMessage(int errorCode,QString command);
    void getRadioList();



    scatParams catParams;
    //int serialP;
    //bool setPTT(bool On);
    //double lastFrequency;
    //QStringList xmlModes;








};

#endif // RIGCONTROL_H
