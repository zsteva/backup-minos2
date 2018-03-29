#include "mqtUtils_pch.h"
#include <QPalette>
#include <QApplication>
#include <QFileDialog>

void appStartup(const QString &appName)
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

    if (!DirectoryExists("./Configuration"))
    {
#ifdef Q_OS_ANDROID
        bool createOK = CreateDir("./Configuration");
        if (!mShowOKCancelMessage(0, createOK?"./Config created":"create ./Config failed; Cancel for abort"))
        {
            exit(0);
        }
#else
        // try for executable directory
        QString fpath = QCoreApplication::applicationDirPath();

        if (DirectoryExists(fpath + "/../Configuration"))
        {
            QDir::setCurrent(fpath + "/..");
        }
        int confTries = 0;
        while (!DirectoryExists("./Configuration") )
        {
            if (++confTries > 2)
            {
                exit(-1);
            }
            QString destDir = QFileDialog::getExistingDirectory(
                        nullptr,
                        "Set Minos Working Directory",
                        fpath,
                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                        );
            if ( !destDir.isEmpty() )
            {

                if (destDir.toUpper().indexOf("/CONFIGURATION") == destDir.size() - QString("/Configuration").size())
                {
                    destDir = destDir.left(destDir.size() - QString("/Configuration").size());
                }
                QDir::setCurrent(destDir);
            }
        }
#endif
    }

    enableTrace( "./TraceLog", appName + "_" );
}

