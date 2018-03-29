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


class RotPreset
{

public:

    RotPreset(int _number, QString _name, QString _bearing);

    int number = 0;
    QString name;
    QString bearing;

};





#endif // ROTATORCOMMON_H
