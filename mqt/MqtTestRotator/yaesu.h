#ifndef YAESU_H
#define YAESU_H

#include <QObject>
#include <QByteArray>


class QString;

// yaesu rotator protocol

// Yaesu Rotator Commands

#define STOP_AZ_ROT "A"
#define RET_CUR_EL  "B"         // Return Current Elevation Value in Degree (format is +0eeeCR)
#define RET_CUR_AZ "C"          // Return Current Azimuth Value in Degree (format is +0aaaCR)
#define RET_CUR_AZ_EL "C2"      // Return Current Azimuth and Elevation Value in Degree (format is +0aaa+0eeeCR)
#define ROT_EL_DOWN "D"
#define STOP_EL_ROT "E"
#define SET_AZ_FULL_CAL "F"     // Set Azimuth Full Scale Calibration
#define SET_EL_FULL_CAL "F2"    // Set Elevation Full Scale Calibration
#define ROT_AZ_CCW "L"
#define ROT_AZ_TO   "M"         // Rotate Azimuth to (MaaaCR)
#define SET_AZ_ZERO_CAL "O"     // Set Azimuth Zero Calibration
#define SET_EL_ZERO_CAL "O2"    // Set Elevation Zero Calibration
#define ROT_AZ_CW   "R"
#define STOP_ALL    "S"
#define ROT_EL_UP   "U"
#define ROT_AZ_EL_TO "W"        // Rotate Azimuth to aaa degrees and Elevation to eee degrees (Waaa eee)

#define CR '\x0D'
#define LF '\x0A'
#define SOM '+'
#define TERM '\x00'


class QSerialPort;

class Yaesu : public QObject
{

    Q_OBJECT

public:
    Yaesu(QSerialPort *serialPort);
    ~Yaesu();

    void request_bearing();
    void rotateClockwise();
    void rotateCClockwise();
public slots:

    void received_bearing();
    void rotate_to_bearing(QString);
    void stop_rotation();


signals:
   void bearing_updated(QString);
   void send_bearing_msg(QByteArray);
   void send_rotate_to_msg(QByteArray);
   void send_stop_msg(QByteArray);
   void send_az_cw_msg(QByteArray);
   void send_az_ccw_msg(QByteArray);
private:

    QByteArray get_bearing_message;
    QByteArray tx_rotate_to_message;
    QByteArray stop_message;
    QByteArray cw_message;
    QByteArray ccw_message;

    bool gotSOM;

    QString command_sent;
    QString bearing_from_controller;
    QString old_bearing_from_controller;

    QSerialPort *m_serialPort;

    QByteArray serialData;
    QByteArray serialBuffer;

    void set_command_sent(QString command);
    void send_to_bearing_display(QByteArray bearing);
    QString get_command_sent();



};

#endif // YAESU_H
