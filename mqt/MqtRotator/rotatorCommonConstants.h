/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) D. G. Balharrie M0DGB/G8FKH
//
//
/////////////////////////////////////////////////////////////////////////////



#ifndef ROTATORCOMMON_H
#define ROTATORCOMMON_H

// Common Rotator Constants

#define DEGREE_SYMBOL '\xB0'
#define ROTATE_MOVE_TIMEOUT 5

#define NUM_ANTENNAS 5

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


// Status messages sent to minos logger
#define STATUS_READY "Ready"
#define STATUS_ROTATE_CCW "Rotating CCW"
#define STATUS_ROTATE_CW "Rotating CW"
#define STATUS_STOP "Stop"
#define STATUS_TURN_TO "Turning to bearing"
#define STATUS_DISCONNECTED "Disconnected"
#define STATUS_ERROR "Error"

// Bearing Log
#define BEARINGLOG_FILETYPE ".log"

#endif // ROTATORCOMMON_H
