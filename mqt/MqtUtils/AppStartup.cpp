#include "mqtUtils_pch.h"

void appStartup(QApplication &a, const QString &appName, bool startLog)
{
    a.setOrganizationName( "Minos2Qt" );
    a.setOrganizationDomain( "g0gjv.org.uk" );
    a.QCoreApplication::setApplicationName( appName );

    QSettings settings;
    QVariant qfont = settings.value( "font" );
    if ( qfont != QVariant() )
    {
        a.setFont( qfont.value<QFont>() );
    }

    if (startLog)
    {
        enableTrace( "./TraceLog", appName + "_" );
    }
}

