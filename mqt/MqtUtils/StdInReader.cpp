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
        if (line.isNull())
            break;          // catch stdin being closed by the other end
        emit stdinLine(line);
    }
}
static bool showServers = true;
bool getShowServers()
{
    return showServers;
}
void setShowServers(bool state)
{
    showServers = state;
}
