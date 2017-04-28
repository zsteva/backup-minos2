#include "bandmapmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BandMapMainWindow w;
    w.show();

    return a.exec();
}
