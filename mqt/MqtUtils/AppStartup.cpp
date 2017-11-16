#include "mqtUtils_pch.h"
#include <QPalette>
#include <QApplication>

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

    QApplication *qa = dynamic_cast<QApplication *>(QApplication::instance());
    qa->setStyleSheet(QString("[readOnly=\"true\"] { background-color: %0 }").arg(qa->palette().color(QPalette::Window).name(QColor::HexRgb)));

    if (startLog)
    {
        enableTrace( "./TraceLog", appName + "_" );
    }
}

