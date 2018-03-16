#include "PubSubValue.h"

QString PubSubValue::psType() const
{
    return _psType;
}

PubSubValue::PubSubValue(QString t):_psType(t)
{

}
PubSubValue::~PubSubValue()
{}
