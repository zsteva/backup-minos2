#include "rigcontrolmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RigControlMainWindow w;
    w.show();

    return a.exec();
}
