#include "ControlMain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ControlMain w;
    w.show();

    return a.exec();
}
