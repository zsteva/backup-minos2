#include "mqtUtils_pch.h"
#include <QPalette>

void appStartup(const QString &appName, bool startLog)
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

    qApp->setStyleSheet(QString("[readOnly=\"true\"] { background-color: %0 }").arg(qApp->palette().color(QPalette::Window).name(QColor::HexRgb)));

    if (startLog)
    {
        enableTrace( "./TraceLog", appName + "_" );
    }
}

