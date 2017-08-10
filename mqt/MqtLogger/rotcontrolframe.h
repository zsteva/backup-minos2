#ifndef ROTCONTROLFRAME_H
#define ROTCONTROLFRAME_H

#include "logger_pch.h"

namespace Ui {
class RotControlFrame;
}

class RotControlFrame : public QFrame
{
    Q_OBJECT

public:
    explicit RotControlFrame(QWidget *parent);
    ~RotControlFrame();

    Ui::RotControlFrame *ui;
    void setRotatorLoaded();

    void setRotatorState(const QString &s);
    void setRotatorBearing(const QString &s);
    void setRotatorAntennaName(const QString &s);
    void setRotatorMaxAzimuth(const QString &s);
    void setRotatorMinAzimuth(const QString &s);


private:

    int getAngle();

    int maxAzimuth = 0;
    int minAzimuth = 0;
    int currentBearing = 0;
    bool moving = false;
    bool movingCW = false;
    bool movingCCW = false;
    bool rot_left_button_status;
    bool rot_right_button_status;
    void clearRotatorFlags();

    bool rotatorLoaded;
    bool isRotatorLoaded();

    void rot_left_button_on();
    void rot_left_button_off();
    void rot_right_button_on();
    void rot_right_button_off();

signals:
    void sendRotator(rpcConstants::RotateDirection direction, int angle );



private slots:

    void on_Rotate_clicked();
    void on_RotateLeft_clicked(bool);
    void on_RotateRight_clicked(bool);
    void on_StopRotate_clicked();


};

#endif // ROTCONTROLFRAME_H
