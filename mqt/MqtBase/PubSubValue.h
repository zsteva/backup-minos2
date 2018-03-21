#ifndef PUBSUBVALUE_H
#define PUBSUBVALUE_H
#include <QString>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include "MinosElement.h"

class PubSubValue
{
protected:
    QString _psType;
public:
    PubSubValue(QString t);
    virtual ~PubSubValue();

    virtual QString pack() const = 0;
    virtual void unpack(QString) = 0;
    QString psType() const;
};

#endif // PUBSUBVALUE_H
