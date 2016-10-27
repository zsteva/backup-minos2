#ifndef ROTCONTROL_H
#define ROTCONTROL_H

#include <QObject>
#include <QComboBox>

#include <hamlib/rotator.h>

#define MAXCONFLEN 128

extern "C" int write_block(hamlib_port_t *p, const char *txbuffer, size_t count);
extern "C" int read_block(hamlib_port_t *p, char *rxbuffer, size_t count);

bool model_Sort(const rot_caps *caps1,const rot_caps *caps2);

//enum serial_parity_e p_none = RIG_PARITY_NONE;
//enum serial_parity_e p_even = RIG_PARITY_EVEN;
//enum serial_parity_e parity_codes[] = {p_none, p_even};




struct srotParams
{
  QString antennaName;
  QString configLabel;
  QString comport; /**<  serial port device*/
  QString rotatorModel;
  QString rotatorModelName;
  int rotatorModelNumber;
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
    bool init(srotParams currentAntenna);
    int getModelNumber(int idx);
    int getRotatorModelIndex();
    bool getRotatorList(QComboBox *cb);
    const char * getMfg_Name(int idx);
    const char * getModel_Name(int idx);
    int rotate_to_bearing(QString bearing);
    int rotateCClockwise(int speed);
    int rotateClockwise(int speed);
    void set_rotatorSpeed(int speed);
    int get_rotatorSpeed();
    azimuth_t getRotatorAzimuth();
    void request_bearing();
    int stop_rotation();
    enum serial_parity_e getSerialParityCode(int index);
    enum serial_handshake_e getSerialHandshakeCode(int index);
    QString initError;


signals:
   void bearing_updated(QString);
//   void send_bearing_msg(QByteArray);
//   void send_rotate_to_msg(QByteArray);
//   void send_stop_msg(QByteArray);
//   void send_az_cw_msg(QByteArray);
//   void send_az_ccw_msg(QByteArray);

private:
    hamlib_port_t myport;
    ROT *my_rot;            // handle to rig (nstance)
    azimuth_t rot_azimuth;  // azimuth from rotator
    elevation_t rot_elevation; // not used
    int rot_speed = 1;
//    freq_t freq;            // frequency
//    rmode_t rmode;          // radio mode of operation
//    pbwidth_t width;
//    vfo_t vfo;              // vfo selection
//    int strength;           // S-Meter level
//   int retcode;            // generic return code from functions
//    rot_model_t myrot_model;

    bool rotControlEnabled;
    void errorMessage(int errorCode,QString command);
    void getRotatorList();
    bool rotatorlistLoaded=false;
    srotParams rotParams;
    int serialP;


//    bool setPTT(bool On);
//    double lastFrequency;
//    QStringList xmlModes;

    int retcode;		/* generic return code from functions */
    int exitcode;

};

#endif // ROTCONTROL_H
