#ifndef PIGPIO_H
#define PIGPIO_H

#include "base_pch.h"
#include <QSocketNotifier>

class PiGPIO;

class GPIOLine: public QObject
{
    Q_OBJECT
    bool input;
    bool value;
    int pin;
    int fd;
    QSharedPointer<QSocketNotifier> pinNotifier;
public:
    GPIOLine(int pin, bool input);
    virtual ~GPIOLine();

    int getfd()
    {
        return fd;
    }
    int getpin()
    {
        return pin;
    }
    bool getValue()
    {
        return value;
    }

    bool initialise();
    void setPin(bool state);
    bool readPin();
private slots:
    void on_activated(int socket);
};

class PiGPIO: public QObject
{
    Q_OBJECT

    QMap<int, QSharedPointer<GPIOLine> > exportedPins;
public:
    PiGPIO();
    virtual ~PiGPIO();
    void setPinInput(int pin);
    void setPinOutput(int pin);

    void setPin(int pin, bool state);
    bool readPin(int pin);

};

#endif // PIGPIO_H
