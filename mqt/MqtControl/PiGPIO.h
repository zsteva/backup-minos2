#ifndef PIGPIO_H
#define PIGPIO_H

#include "base_pch.h"
#include <QSocketNotifier>

class PiGPIO;

class GPIOLine: public QObject
{
    Q_OBJECT
    bool input;
    int pin;
    int fd;
public:
    GPIOLine(int pin, bool input);
    virtual ~GPIOLine() override;

    int getfd()
    {
        return fd;
    }
    int getpin()
    {
        return pin;
    }


    bool initialise();
    void setPin(bool state);
    bool readPin();

};

class PiGPIO: public QObject
{
    Q_OBJECT

    QMap<int, QSharedPointer<GPIOLine> > exportedPins;
public:
    PiGPIO();
    virtual ~PiGPIO() override;
    void setPinInput(int pin);
    void setPinOutput(int pin);

    void setPin(int pin, bool state);
    bool readPin(int pin);

};

#endif // PIGPIO_H
