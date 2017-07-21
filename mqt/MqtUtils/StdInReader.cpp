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
}

void StdInReader::run()
{
    QTextStream stdinStream(stdin, QIODevice::ReadOnly);

    QString line;
    for (;;)
    {
        line = stdinStream.readLine();
        emit stdinLine(line);
    }
}
