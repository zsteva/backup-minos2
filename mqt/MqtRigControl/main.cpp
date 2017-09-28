/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rig Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2017
//
//
//
/////////////////////////////////////////////////////////////////////////////







#include "rigcontrolmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{


    QApplication a(argc, argv);
    appStartup(a, "MinosQtRigControl");


    RigControlMainWindow w;
    w.show();

    return a.exec();
}
