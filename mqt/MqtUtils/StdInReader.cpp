#include "mqtUtils_pch.h"
#include "StdInReader.h"

StdInReader::StdInReader():stream(stdin, QIODevice::ReadOnly)
{
    trace("Arguments");
    foreach (QString arg, QCoreApplication::instance()->arguments())
    {
        trace(arg);
    }
    trace("End of Arguments");

    trace("StdIn");

    QString line;
    while (stream.readLineInto(&line))
    {
        trace(line);
        if (line.compare("CloseStdin", Qt::CaseInsensitive) == 0)
            break;
    }
    trace("End of StdIn");
}
