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
   QString loggerRadio = "";

    QApplication a(argc, argv);
    appStartup(a, "MinosQtRigControl");

    if (argc == 2)
    {
        loggerRadio = QCoreApplication::arguments().at(1);
    }

    RigControlMainWindow w(loggerRadio);
    w.show();

    return a.exec();
}
