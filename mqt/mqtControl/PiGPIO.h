#ifndef PIGPIO_H
#define PIGPIO_H


class PiGPIO
{
    QVector<int> exportedPins;
public:
    PiGPIO();
    ~PiGPIO();
    void setPinInput(int pin);
    void setPinOutput(int pin);

    void setPin(int pin, bool state);
    bool readPin(int pin);
};

#endif // PIGPIO_H
