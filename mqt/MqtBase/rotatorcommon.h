/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2018
//
//
//
/////////////////////////////////////////////////////////////////////////////

#ifndef ROTATORCOMMON_H
#define ROTATORCOMMON_H

#include <QString>
#include <QStringList>

class RotPresetData
{

public:

    RotPresetData(int _number, QString _name, QString _bearing);
    RotPresetData();

    int number = 0;
    QString name;
    QString bearing;

};


// Common Rotator Constants

#define DEGREE_SYMBOL '\xB0'
#define ROTATE_MOVE_TIMEOUT 5

//#define NUM_ANTENNAS 5

// Rotator Keys

#define ROTATE_CW_KEY "Ctrl+r"
#define ROTATE_CCW_KEY "Ctrl+l"
//#define ROTATE_CW_KEY Qt::CTRL + Qt::Key_Right
//#define ROTATE_CCW_KEY Qt::CTRL + Qt::Key_Left
#define ROTATE_STOP_KEY "Ctrl+s"
#define ROTATE_TURN_KEY "Ctrl+t"

#define COMPASS_MAX360 360
#define COMPASS_HALF 180
#define COMPASS_MIN0 0
#define COMPASS_ERROR 999

#define ROTATE_ENDSTOP_TOLERANCE 0


// Status messages sent to minos logger
#define ROT_STATUS_CONNECTED "Connected"
#define ROT_STATUS_DISCONNECTED "Disconnected"

#define ROT_STATUS_ROTATE_CCW "<font color='Green'>Rotating CCW</font>"
#define ROT_STATUS_ROTATE_CW "<font color='Green'>Rotating CW</font>"
#define ROT_STATUS_STOP "Stop"
#define ROT_STATUS_TURN_TO "<font color='Green'>Turning to bearing</font>"
#define ROT_STATUS_DISCONNECTED "Disconnected"
#define ROT_STATUS_ERROR "Error"

#define ROT_ACTIVE "Rot_Active"
#define ROT_NOT_ACTIVE "Rot_Not_Active"

// Bearing Log
#define BEARINGLOG_FILETYPE ".log"

// File Name Constants

const QString LOCAL_ANTENNA = "Local";

const QString CONFIGURATION_FILEPATH_LOGGER = "./Configuration/";
const QString CONFIGURATION_FILEPATH_LOCAL = "./Configuration/";

const QString MINOS_ROTATOR_CONFIG_FILE = "MinosRotatorConfig.ini";
const QString ROTATOR_DATA_FILE = "rotators.ini";

const QString ANTENNA_PATH_LOGGER = "./Configuration/Antenna/";
const QString ANTENNA_PATH_LOCAL = "./Configuration/Antenna/";
const QString FILENAME_AVAIL_ANTENNAS = "AvailAntenna.ini";
const QString FILENAME_CURRENT_ANTENNA = "CurrentAntenna.ini";

// Rotator Types
// ROT_180_180 is ROT_0_360 set to southstop.
enum endStop {ROT_NEG180_180, ROT_0_360, ROT_0_450, ROT_NEG180_540, ROT_180_180};
const QStringList endStopNames = (QStringList() << "ROT_NEG180_180" << "ROT_0_360" << "ROT_0_450" << "ROT_NEG180_540" << "ROT_180_180");



// Overlap Status

enum overlapStat { NO_OVERLAP, NEG_OVERLAP, POS_OVERLAP};

// Pushbutton Styles

const QString BUTTON_ON_STYLE = QString("background-color: Sandybrown ; border-style: outset; border-width: 1px; border-color: black; min-width: 5em; padding: 3px;\n");
const QString BUTTON_OFF_STYLE = QString("background-color: Gainsboro ; border-style: outset; border-width: 1px; border-color: black; min-width: 5em; padding: 3px;\n");


#endif // ROTATORCOMMON_H
