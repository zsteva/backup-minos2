#include "rotatormainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RotatorMainWindow w;
    w.show();

    return a.exec();
}
