#include "mqtUtils_pch.h"

void appStartup(QApplication &a, const QString &appName, bool startLog)
{
    a.setOrganizationName( "Minos2Qt" );
    a.setOrganizationDomain( "g0gjv.org.uk" );
    a.QCoreApplication::setApplicationName( appName );

    if (startLog)
    {
        enableTrace( "./TraceLog", appName + "_" );
    }
}

