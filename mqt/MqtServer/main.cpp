#include "servermain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName( "G0GJV" );
    a.setOrganizationDomain( "g0gjv.org.uk" );
    a.QCoreApplication::setApplicationName( "MinosQtServer" );

    ServerMain w;
    w.show();

    return a.exec();
}
