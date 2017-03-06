#include "base_pch.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "PiGPIO.h"

// derived from http://elinux.org/RPi_GPIO_Code_Samples

#define BUFFER_MAX 3

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define PIN  24 /* P1-18 */
#define POUT 4  /* P1-07 */

static int
GPIOExport(int pin)
{
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (-1 == fd) {
        trace("Failed to open export for writing!");
        return(-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);
    return(0);
}

static int
GPIOUnexport(int pin)
{
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (-1 == fd) {
        trace ("Failed to open unexport for writing!");
        return(-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);
    return(0);
}

static int
GPIODirection(int pin, int dir)
{
    static const char s_directions_str[]  = "in\0out";

#define DIRECTION_MAX 35
    char path[DIRECTION_MAX];
    int fd;

    snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
        trace("Failed to open gpio direction for writing!");
        return(-1);
    }

    if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
        trace("Failed to set direction!");
        return(-1);
    }

    close(fd);
    return(0);
}

static int
GPIORead(int pin)
{
#define VALUE_MAX 30
    char path[VALUE_MAX];
    char value_str[3];
    int fd;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_RDONLY);
    if (-1 == fd) {
        trace("Failed to open gpio value for reading!");
        return(-1);
    }

    if (-1 == read(fd, value_str, 3)) {
        trace("Failed to read value!");
        return(-1);
    }

    close(fd);

    return(atoi(value_str));
}

static int
GPIOWrite(int pin, int value)
{
    static const char s_values_str[] = "01";

    char path[VALUE_MAX];
    int fd;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
        trace("Failed to open gpio value for writing!");
        return(-1);
    }

    if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
        trace("Failed to write value!");
        return(-1);
    }

    close(fd);
    return(0);
}
PiGPIO::PiGPIO()
{

}
PiGPIO::~PiGPIO()
{
    foreach(int pin, exportedPins)
    {
        GPIOUnexport(pin);
    }
    exportedPins.clear();
}

void PiGPIO::setPinInput(int pin)
{
    if (GPIOExport(pin) < 0)
        return;
    if (GPIODirection(pin, IN) < 0)
        return;
}

void PiGPIO::setPinOutput(int pin)
{
    if (GPIOExport(pin) < 0)
        return;
    if (GPIODirection(pin, OUT) < 0)
        return;
    if (!exportedPins.contains(pin))
    {
        exportedPins.push_back(pin);
    }
}

void PiGPIO::setPin(int pin, bool state)
{
    GPIOWrite(pin, state);
}

bool PiGPIO::readPin(int pin)
{
    return GPIORead(pin);
}
