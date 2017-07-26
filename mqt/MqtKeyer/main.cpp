#include "KeyerMain.h"
#include <QApplication>
#include "AppStartup.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    appStartup(a, "MinosQtKeyer");

    KeyerMain w;
    w.show();

    return a.exec();
}
