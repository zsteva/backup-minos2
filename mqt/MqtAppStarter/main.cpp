#include "MainWindow.h"
#include <QApplication>
#include "AppStartup.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    appStartup("MinosQtAppStarter");

    MainWindow w;
    w.show();

    return a.exec();
}
