#include "ControlMain.h"
#include <QApplication>
#include "AppStartup.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    appStartup("MinosQtControl");

    ControlMain w;
    w.show();

    return a.exec();
}
