#include "minos_pch.h"
#include "servermain.h"
#include "AppStartup.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    appStartup("MinosQtServer");

    ServerMain w;
    w.show();

    return a.exec();
}
