#ifndef ROTCONTROL_H
#define ROTCONTROL_H

#include <QObject>

class rotcontrol : public QObject
{
    Q_OBJECT
public:
    explicit rotcontrol(QObject *parent = 0);

signals:

public slots:
};

#endif // ROTCONTROL_H