#include "logger_pch.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName( "G0GJV" );
    a.setOrganizationDomain( "g0gjv.org.uk" );
    a.QCoreApplication::setApplicationName( "MinosQtLogger" );

    QSettings settings;
    QVariant qfont = settings.value( "font" );
    if ( qfont != QVariant() )
    {
        a.setFont( qfont.value<QFont>() );
    }
    QSharedMemory mem( "MinosQtLoggerSingleApplication" );
    if ( !mem.create( 1 ) )
    {
        QMessageBox::critical( 0, "Minos Qt Logger Instance detected!", "Application is already running!\nApplication terminating...", "Ok" );
        exit( 0 );
    }

    TLogContainer w;
    w.show();

    return a.exec();
}
