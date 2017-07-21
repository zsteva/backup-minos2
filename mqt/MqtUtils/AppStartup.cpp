#include "AppStartup.h"

void appStartup(QApplication &a, const QString &appName)
{
    a.setOrganizationName( "Minos2Qt" );
    a.setOrganizationDomain( "g0gjv.org.uk" );
    a.QCoreApplication::setApplicationName( appName );
}

