#include "KeyerMain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KeyerMain w;
    w.show();

    return a.exec();
}
