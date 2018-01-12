/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
// Hamlib library
//
//
/////////////////////////////////////////////////////////////////////////////






#ifndef ROTCONTROL_H
#define ROTCONTROL_H

#include <QObject>
#include <QComboBox>
#include <QStringList>

#include <hamlib/rotator.h>
#include <hamlib/rig.h>         // for debug

#define MAXCONFLEN 128

extern "C" int write_block(hamlib_port_t *p, const char *txbuffer, size_t count);
extern "C" int read_block(hamlib_port_t *p, char *rxbuffer, size_t count);

bool model_Sort(const rot_caps *caps1,const rot_caps *caps2);
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
                                "Memory shortage", "Function not implemented",
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
                                "RIG_EDOM",
                                "Hamlib Init Fail"};


}





class srotParams
{
public:
  QString antennaName;
  QString antennaNumber;
  QString configLabel;
  QString comport;
  QString rotatorModel;
  QString rotatorManufacturer;
  QString rotatorModelName;
  int rotatorModelNumber = 0;
  QString pollInterval;         // ms
  azimuth_t min_azimuth = 0.0;
  azimuth_t max_azimuth = 0.0;
  elevation_t min_elevation = 0.0;
  elevation_t max_elevation = 0.0;
  bool southStopFlag = false;
  bool overRunFlag = false;
  int antennaOffset = 0;
  bool moving = false;
  int portType = int(RIG_PORT_NONE);
  QString networkAdd;
  QString networkPort;
  int serial_rate_max = 0;
  int serial_rate_min = 0;
  int baudrate = 0;
  int parity = 0;
  int stopbits = 0;
  int databits = 0;
  int handshake = 0;
  bool enableRot = false;
  bool activeRTS = false;
  bool activeDTR = false;
  bool nactiveRTS = false;
  bool nactiveDTR = false;

};




class RotControl: public QObject
{
    Q_OBJECT

public:
    explicit RotControl(QObject *parent = 0);
    ~RotControl();
    int init(srotParams currentAntenna);
    int closeRotator();
    int getModelNumber(int idx);
    int getRotatorModelIndex();
    void getRotatorList();
    bool getRotatorList(QComboBox *cb);
    const char * getMfg_Name(int idx);
    const char * getModel_Name(int idx);
    void set_rotatorSpeed(int speed);
    int get_rotatorSpeed();
    void set_serialConnected(bool connectFlag);
    bool get_serialConnected();
    azimuth_t getRotatorAzimuth();
    int request_bearing();
    int rotate_to_bearing(int bearing);
    int rotateCClockwise(int speed);
    int rotateClockwise(int speed);
    int stop_rotation();
    serial_parity_e getSerialParityCode(int index);
    serial_handshake_e getSerialHandshakeCode(int index);
    QStringList getParityCodeNames();
    QStringList getHandShakeNames();
    QStringList getBaudRateNames();
    QStringList getDataBitsNames();
    QStringList getStopBitsNames();
    QStringList gethamlibErrorMsg();
    QString gethamlibErrorMsg(int errorCode);
    QString gethamlibVersion();
//    QString initError;

    azimuth_t getMaxAzimuth();
    azimuth_t getMinAzimuth();
    elevation_t getMaxElevation();
    elevation_t getMinElevation();

    int calcSouthBearing(int rotatorBearing);

    int rig_message_cb(enum rig_debug_level_e debug_level, const char *fmt, va_list ap);


    int getMaxBaudRate();
    int getMinBaudRate();
    int getSupportCwCcwCmd(int rotNumber, bool *flag);
    int getMaxMinRotation(int rotNumber, int *maxRot, int *minRot);
    int getPortType(int rotNumber, rig_port_e *portType);

    int getModelInfo(QString rotModel, int *rotModelNumber, QString *rotMfgName, QString *rotModelName);

signals:
   void bearing_updated(int);
   void request_bearingError(int);
   void debug_protocol(QString);


private:
    hamlib_port_t myport;
    ROT *my_rot = 0;            // handle to rig instance)
    azimuth_t rot_azimuth;  // azimuth from rotator
    elevation_t rot_elevation; // not used

    int rot_speed = 100;
    bool rotControlEnabled;
    bool serialConnected;
    void errorMessage(int errorCode,QString command);
    bool rotatorlistLoaded=false;
    srotParams curRotParams;
    int serialP;



    int retcode;		/* generic return code from functions */
    int exitcode;



};

#endif // ROTCONTROL_H
