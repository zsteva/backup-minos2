#include "chatmain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TMinosChatForm w;
    w.show();

    return a.exec();
}
