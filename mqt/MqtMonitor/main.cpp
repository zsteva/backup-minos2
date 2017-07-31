#include "MonitorMain.h"
#include <QApplication>
#include "AppStartup.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    appStartup(a, "MinosQtMonitor");

    MonitorMain w;
    w.show();

    return a.exec();
}
