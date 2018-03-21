#include "AntennaDetail.h"
#include "RPCCommandConstants.h"
#include "MTrace.h"

void AntennaDetail::setMinAzimuth(int minAzimuth)
{
    _minAzimuth.setValue( minAzimuth);
}

void AntennaDetail::setMaxAzimuth(int maxAzimuth)
{
    _maxAzimuth.setValue(maxAzimuth);
}

AntennaDetail::AntennaDetail(QString s):PubSubValue(AntennaDetailType)
{
    unpack(s);
}
AntennaDetail::~AntennaDetail()
{
    qRegisterMetaType< AntennaDetail > ( "AntennaDetail" );

}
bool AntennaDetail::isDirty()
{
    return (_minAzimuth.isDirty() || _maxAzimuth.isDirty());
}
void AntennaDetail::clearDirty()
{
    _minAzimuth.clearDirty();
    _maxAzimuth.clearDirty();
}
QString AntennaDetail::pack() const
{
    QJsonObject jv;

    jv.insert(rpcConstants::rotatorMinAzimuth, minAzimuth());
    jv.insert(rpcConstants::rotatorMaxAzimuth, maxAzimuth());

    QJsonDocument json(jv);

    QString message(json.toJson(QJsonDocument::Compact));

    return message;
}
void AntennaDetail::unpack(QString s)
{
    QJsonParseError err;
    QJsonDocument json = QJsonDocument::fromJson(s.toUtf8(), &err);
    if (!err.error)
    {
        _minAzimuth.setValue(json.object().value(rpcConstants::rotatorMinAzimuth).toInt());
        _maxAzimuth.setValue(json.object().value(rpcConstants::rotatorMaxAzimuth).toInt());
    }
    else
    {
        trace("Err " + err.errorString() + " Bad Json document " + s);
    }

}
int AntennaDetail::minAzimuth() const
{
    return _minAzimuth.getValue();
}

int AntennaDetail::maxAzimuth() const
{
    return _maxAzimuth.getValue();
}
