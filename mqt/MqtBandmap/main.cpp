#include "bandmapmainwindow.h"
#include <QApplication>
#include "AppStartup.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    appStartup("MinosQtBandmap");

    BandMapMainWindow w;
    w.show();

    return a.exec();
}
