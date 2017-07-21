#include "base_pch.h"
#include "chatmain.h"
#include <QApplication>
#include "AppStartup.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    appStartup(a, "MinosQtChat");

    TMinosChatForm w;
    w.show();

    return a.exec();
}
