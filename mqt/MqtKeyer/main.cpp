#include "KeyerMain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName( "Minos2Qt" );
    a.setOrganizationDomain( "g0gjv.org.uk" );
    a.QCoreApplication::setApplicationName( "MinosQtKeyer" );

    KeyerMain w;
    w.show();

    return a.exec();
}
