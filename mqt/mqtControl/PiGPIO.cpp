#include "base_pch.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "PiGPIO.h"

GPIOLine::GPIOLine(int pin, bool input):pin(pin), input(input), fd(-1)
{

}

GPIOLine::~GPIOLine()
{
    close(fd);
    QString spin = "/sys/class/gpio/unexport";

    fd = open(spin.toLatin1(), O_WRONLY);
    if (-1 == fd)
    {
        trace("Failed to open " + spin + " for writing!");
        return;
    }

    spin = QString::number(pin);
    QByteArray apin = spin.toUtf8();
    write(fd, apin.data(), apin.length());
    close(fd);
}
bool GPIOLine::initialise()
{
    QString spin = "/sys/class/gpio/export";

    fd = open(spin.toLatin1(), O_WRONLY);
    if (-1 == fd)
    {
        trace("Failed to open " + spin + " for writing!");
        return false;
    }

    spin = QString::number(pin);
    QByteArray apin = spin.toUtf8();
    write(fd, apin.data(), apin.length());
    close(fd);

    spin = "/sys/class/gpio/gpio" + spin;
    fd = open((spin + "/direction").toLatin1(), O_WRONLY);
    if (-1 == fd)
    {
        trace("Failed to open" + spin  + "/direction for writing!");
        return(-1);
    }

    spin = input?"in":"out";
    apin = spin.toUtf8();
    if (-1 == write(fd, apin.data(), apin.length()))
    {
        trace("Failed to set direction!");
        return(-1);
    }

    close(fd);

    fd = open((spin + "/value").toLatin1(), O_RDWR);
    if (-1 == fd)
    {
        trace("Failed to open " + spin + "/value for read/write!");
        return false;
    }

    if (input)
    {
        pinNotifier = QSharedPointer<QSocketNotifier>(new QSocketNotifier(fd, QSocketNotifier::Read));
        connect(pinNotifier.data(), SIGNAL(activated(int)), this, SLOT(on_activated(int)));

        value = readPin();
    }
    return true;
}
void GPIOLine::setPin(bool state)
{
    if (1 != write(fd, state ? "1" : "0", 1))
    {
        trace("Failed to write value pin " + QString::number(pin));
    }
}

bool GPIOLine::readPin()
{
    char value_str[3];

    if (-1 == read(fd, value_str, 3))
    {
        trace("Failed to read value for pin " + QString::number(pin));
        return false;
    }
    return value_str[0] != '0';
}
void GPIOLine::on_activated(int /*socket*/)
{
    value = readPin();
}

PiGPIO::PiGPIO()
{

}
PiGPIO::~PiGPIO()
{
    exportedPins.clear();
}

void PiGPIO::setPinInput(int pin)
{
    QSharedPointer<GPIOLine> line(new GPIOLine(pin, true));
    if (line->initialise())
        exportedPins[pin] = line;
}

void PiGPIO::setPinOutput(int pin)
{
    QSharedPointer<GPIOLine> line(new GPIOLine(pin, false));
    if (line->initialise())
        exportedPins[pin] = line;
}

void PiGPIO::setPin(int pin, bool state)
{
    QSharedPointer<GPIOLine> line = exportedPins[pin];
    if (line)
        line->setPin(state);
}

bool PiGPIO::readPin(int pin)
{
    QSharedPointer<GPIOLine> line = exportedPins[pin];
    if (line)
        return line->getValue();    // read the buffered value
    return false;
}

