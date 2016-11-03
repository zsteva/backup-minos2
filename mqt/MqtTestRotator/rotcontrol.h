#ifndef ROTCONTROL_H
#define ROTCONTROL_H

#include <QObject>
#include <QComboBox>
#include <QStringList>

#include <hamlib/rotator.h>

#define MAXCONFLEN 128

extern "C" int write_block(hamlib_port_t *p, const char *txbuffer, size_t count);
extern "C" int read_block(hamlib_port_t *p, char *rxbuffer, size_t count);

bool model_Sort(const rot_caps *caps1,const rot_caps *caps2);


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





struct srotParams
{
  QString antennaName;
  QString configLabel;
  QString comport; /**<  serial port device*/
  QString rotatorModel;
  QString rotatorModelName;
  int rotatorModelNumber;
  azimuth_t max_azimuth;
  elevation_t min_elevation;
  bool moving;
  int serial_rate_max;
  int serial_rate_min;
  int baudrate; /**<  serial port baudrate*/
  int parity;
  int stopbits;
  int databits;
  int handshake;
  bool enableRot;
  bool activeRTS;
  bool activeDTR;
  bool nactiveRTS;
  bool nactiveDTR;

};




class RotControl: public QObject
{
    Q_OBJECT
public:
    RotControl();
    ~RotControl();
    int init(srotParams currentAntenna);
    int closeRotator();
    int getModelNumber(int idx);
    int getRotatorModelIndex();
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
    QString initError;


signals:
   void bearing_updated(QString);
   void request_bearingError(int);


private:
    hamlib_port_t myport;
    ROT *my_rot;            // handle to rig (nstance)
    azimuth_t rot_azimuth;  // azimuth from rotator
    elevation_t rot_elevation; // not used
    int rot_speed = 100;
    bool rotControlEnabled;
    bool serialConnected;
    void errorMessage(int errorCode,QString command);
    void getRotatorList();
    bool rotatorlistLoaded=false;
    srotParams rotParams;
    int serialP;


    int retcode;		/* generic return code from functions */
    int exitcode;

};

#endif // ROTCONTROL_H
