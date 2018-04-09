#include "base_pch.h"
#include "AntennaDetail.h"

AntennaDetail::AntennaDetail(): PubSubValue(AntennaDetailType)
{
    _maxAzimuth.setInitialValue(0);
    _minAzimuth.setInitialValue(0);
}
AntennaDetail::AntennaDetail(QString s):PubSubValue(AntennaDetailType)
{
    qRegisterMetaType< AntennaDetail > ( "AntennaDetail" );
    _maxAzimuth.setInitialValue(0);
    _minAzimuth.setInitialValue(0);
    unpack(s);
}
bool AntennaDetail::isDirty() const
{
    return (_minAzimuth.isDirty() || _maxAzimuth.isDirty());
}
void AntennaDetail::clearDirty()
{
    _minAzimuth.clearDirty();
    _maxAzimuth.clearDirty();
}
void AntennaDetail::setDirty()
{
    _minAzimuth.setDirty();
    _maxAzimuth.setDirty();
}
QString AntennaDetail::pack() const
{
    QJsonObject jv;

    jv.insert(rpcConstants::rotatorMinAzimuth, minAzimuth().getValue());
    jv.insert(rpcConstants::rotatorMaxAzimuth, maxAzimuth().getValue());

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

void AntennaDetail::setMinAzimuth(int minAzimuth)
{
    _minAzimuth.setValue( minAzimuth);
}

void AntennaDetail::setMaxAzimuth(int maxAzimuth)
{
    _maxAzimuth.setValue(maxAzimuth);
}


MinosItem<int> AntennaDetail::minAzimuth() const
{
    return _minAzimuth;
}

MinosItem<int> AntennaDetail::maxAzimuth() const
{
    return _maxAzimuth;
}


