#include "base_pch.h"
#include "PubSubValue.h"
#include "ConfigFile.h"

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
PubSubName::PubSubName()
{}

PubSubName::PubSubName(const AnalysePubSubNotify &an)
{
    _server = an.getPublisherServer();
    _appName = an.getPublisherProgram();
    _key = an.getKey();
}
PubSubName::PubSubName(const QString &s)
{
    if (s.isEmpty())
        return;

    QStringList sl = s.split('/');
    std::reverse(sl.begin(), sl.end());
    if (sl.size())
    {
        _key = sl[0];
        if (sl.size() > 1)
        {
            _appName = sl[1];
            if (sl.size() > 2)
            {
                _server = sl[2];
            }
        }

        if (_key.isEmpty())
            return;
        if (_server.isEmpty())
        {
            _server = MinosConfig::getMinosConfig()->getThisServerName();
        }
        if (_appName.isEmpty())
        {
            MinosRPC *rpc = MinosRPC::validMinosRPC();
            if (rpc)
            {
                _appName = rpc->getAppName();
            }
        }
    }
}
bool PubSubName::isEmpty()
{
    return _server.isEmpty() && _appName.isEmpty() && _key.isEmpty();
}
QString PubSubName::toString()
{
    if (isEmpty())
        return QString();

    return _server + "/" + _appName + "/" + _key;
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
bool PubSubName::operator!= ( const PubSubName& rhs ) const
{
    return !(*this == rhs);
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
