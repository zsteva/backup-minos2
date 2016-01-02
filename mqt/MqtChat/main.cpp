#include "base_pch.h"
#include "chatmain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName( "G0GJV" );
    a.setOrganizationDomain( "g0gjv.org.uk" );
    a.QCoreApplication::setApplicationName( "MinosQtChat" );

    TMinosChatForm w;
    w.show();

    return a.exec();
}
