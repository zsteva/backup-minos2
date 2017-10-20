#include "mqtUtils_pch.h"

void appStartup(QApplication &/*a*/, const QString &appName, bool startLog)
{
    QApplication::setOrganizationName( "Minos2Qt" );
    QApplication::setOrganizationDomain( "g0gjv.org.uk" );
    QApplication::QCoreApplication::setApplicationName( appName );

    QSettings settings;
    QVariant qfont = settings.value( "font" );
    if ( qfont != QVariant() )
    {
        QApplication::setFont( qfont.value<QFont>() );
    }

    if (startLog)
    {
        enableTrace( "./TraceLog", appName + "_" );
    }
}

