#include "bandmapmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName( "Minos2Qt" );
    a.setOrganizationDomain( "g0gjv.org.uk" );
    a.QCoreApplication::setApplicationName( "MinosQtBandmap" );

    BandMapMainWindow w;
    w.show();

    return a.exec();
}
