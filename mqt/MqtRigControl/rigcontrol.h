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
                                "Invalid parameter", "Invalid configuration",
                                "Memory shortage", "Function not implemented",
                                "Communication timed out", "IO error, including open failed",
                                "Internal Hamlib error", "Protocol error",
                                "Command rejected by the rig", "Command performed, but arg truncated",
                                "Function not available", "VFO not targetable",
                                "Error talking on the bus", "Collision on the bus",
                                "NULL RIG handle or any invalid pointer parameter in get arg",
                                "Invalid VFO", "RIG_EDOM"};


}

// This was the hamlib catParams structure, other fields have been added
// to support other functions.

struct scatParams
{
//  QString configLabel;
  QString radioName;
  QString radioNumber;
  QString comport; /**<  serial port device*/
  QString radioMfg_Name;
  QString radioModel;
  QString radioModelName;
  int radioModelNumber;
  QString civAddress;
  int baudrate; /**<  serial port baudrate*/
  int parity;
  int stopbits;
  int databits;
  int handshake;
  bool enableCAT;
  bool enableSerialPTT;
  QString pttSerialPort;
  bool activeRTS;
  bool activeDTR;
  bool nactiveRTS;
  bool nactiveDTR;
  ptt_type_t pttType;
//  bool enableXMLRPC;
//  int XMLRPCPort;
  double txOnDelay;
  QString transVertOffsetStr;
  freq_t transVertOffset;
  bool transVertEnable;
  bool transVertNegative;

};


class RigControl : public QObject
{
    Q_OBJECT
public:
    explicit RigControl(QObject *parent = 0);
    ~RigControl();
    int init(scatParams currentRadio);
    bool enabled() {return rigControlEnabled;}

    int getFrequency(vfo_t vfo, freq_t *frequency);
    int setFrequency(freq_t frequency, vfo_t vfo);

    int getMode(vfo_t vfo, rmode_t *mode, pbwidth_t *width);
    int setMode(vfo_t vfo, rmode_t mode, pbwidth_t width);
    QString convertModeQstr(rmode_t mode);

    int getVfo(vfo_t *vfo);
    int setVfo(vfo_t vfo);
    QString convertVfoQStr(vfo_t vfo);

    int getModelNumber(int idx);
    int getRigModelIndex();
    void getRigList();
    bool getRigList(QComboBox *cb);
    const char * getMfg_Name(int idx);
    const char * getModel_Name(int idx);
    void disable();
    int closeRig();
    scatParams* params() {return &catParams;}
    void activatePTT(bool b);
    double getTxDelay() {return catParams.txOnDelay;}
    int rawCommand(QByteArray ba);
  //  QString initError;
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

    int rig_message_cb(enum rig_debug_level_e debug_level, const char *fmt, va_list ap);

  signals:
    void frequency_updated(double);
    void debug_protocol(QString);

  private:
    hamlib_port_t myport;
    RIG *my_rig;            // handle to rig instance
//    freq_t frequency;            // frequency
//    rmode_t rmode;          // radio mode of operation
//    pbwidth_t width;
//    vfo_t vfo;              // vfo selection
//    int strength;           // S-Meter level
//    int retcode;            // generic return code from functions
    rig_model_t myrig_model;
    bool rigControlEnabled;
    bool serialConnected;
    void errorMessage(int errorCode,QString command);
    void getRadioList();



    scatParams catParams;
    int serialP;
    bool setPTT(bool On);
    double lastFrequency;
    QStringList xmlModes;




};

#endif // RIGCONTROL_H
