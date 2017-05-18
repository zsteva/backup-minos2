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

// Rotator Keys
#define ROTATE_CW_KEY Qt::Key_R
#define ROTATE_CCW_KEY Qt::Key_L
//#define ROTATE_CW_KEY Qt::Key_Right
//#define ROTATE_CCW_KEY Qt::Key_Left
#define ROTATE_STOP_KEY Qt::Key_S
#define ROTATE_TURN_KEY Qt::Key_T

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
