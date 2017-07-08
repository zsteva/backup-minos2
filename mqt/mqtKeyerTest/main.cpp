#include "mqtktMainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mqtktMainWindow w;
    w.show();

    return a.exec();
}
