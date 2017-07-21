#include "mqtUtils_pch.h"
#include <QTextStream>

#include "StdInReader.h"

StdInReader::StdInReader()
{
    trace("Arguments");
    foreach (QString arg, QCoreApplication::instance()->arguments())
    {
        trace(arg);
    }
    trace("End of Arguments");

    start();

}

void StdInReader::run()
{
    QTextStream stdinStream(stdin, QIODevice::ReadOnly);

    QString line;
    for (;;)
    {
        line = stdinStream.readLine();
        trace("StdIn");
        trace(line);
        emit stdinLine(line);
        trace("End of StdIn");
    }
    trace("Thread exiting");
}
