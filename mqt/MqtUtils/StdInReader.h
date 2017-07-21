#ifndef STDINREADER_H
#define STDINREADER_H
#include <QThread>

class StdInReader: public QThread
{
    Q_OBJECT

    virtual void run();
public:
    StdInReader();

signals:
    void stdinLine(QString);
};

#endif // STDINREADER_H
