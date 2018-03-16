#ifndef RIGCACHE_H
#define RIGCACHE_H
#include <QMap>
#include "RigState.h"

class RigCache
{
public:
    RigCache();

    QMap<QString, RigState> rigStates;

    void setState(QString name, RigState &state);
};

#endif // RIGCACHE_H
