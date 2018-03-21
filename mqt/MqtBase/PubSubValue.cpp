#include "base_pch.h"
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
//=======================================================================
PubSubName::PubSubName(const AnalysePubSubNotify &an)
{
    _server = an.getPublisherServer();
    _appName = an.getPublisherProgram();
    _key = an.getKey();

}

bool PubSubName::operator< ( const PubSubName& rhs ) const
{
    int scomp = _server.compare(rhs.server(), Qt::CaseInsensitive);
    if (scomp == 0)
    {
        int acomp = _appName.compare(rhs.appName(), Qt::CaseInsensitive);
        if (acomp == 0)
            return _key.compare(rhs.key(), Qt::CaseInsensitive) < 0;
        return acomp < 0;
    }
    else
        return scomp < 0;
}
bool PubSubName::operator== ( const PubSubName& rhs ) const
{
    int scomp = _server.compare(rhs.server(), Qt::CaseInsensitive);
    if (scomp == 0)
    {
        int acomp = _appName.compare(rhs.appName(), Qt::CaseInsensitive);
        if (acomp == 0)
            return _key.compare(rhs.key(), Qt::CaseInsensitive) == 0;
    }
    return false;
}


QString PubSubName::appName() const
{
    return _appName;
}

void PubSubName::setAppName(const QString &appName)
{
    _appName = appName;
}

QString PubSubName::key() const
{
    return _key;
}

void PubSubName::setKey(const QString &key)
{
    _key = key;
}

QString PubSubName::server() const
{
    return _server;
}

void PubSubName::setServer(const QString &server)
{
    _server = server;
}
