#ifndef ROTCONTROL_H
#define ROTCONTROL_H

#include <QObject>
#include <QComboBox>

#include <hamlib/rotator.h>

#define MAXCONFLEN 128

extern "C" int write_block(hamlib_port_t *p, const char *txbuffer, size_t count);
extern "C" int read_block(hamlib_port_t *p, char *rxbuffer, size_t count);

bool model_Sort(const rot_caps *caps1,const rot_caps *caps2);




struct srotParams
{
  QString antennaName;
  QString configLabel;
  QString comport; /**<  serial port device*/
  int comport_idx;
  QString rotatorModel;
  int rotatorModelNumber;
  int rotatorModel_idx;
  int baudrate; /**<  serial port baudrate*/
  int baudrate_idx;
  QString parity;
  int parity_idx;
  int stopbits;
  int stopbits_idx;
  int databits;
  int databits_idx;
  QString handshake;
  int handshake_idx;
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

    int getModelNumber(int idx);
    int getRotatorModelIndex();
    bool getRotatorList(QComboBox *cb);
    const char * getMfg_Name(int idx);
    const char * getModel_Name(int idx);
    QString initError;

private:
    hamlib_port_t myport;
    ROT *my_rot;            // handle to rig (nstance)
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
