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

void AntennaDetail::setPresets(const QString &p)
{
    _presets.setValue( p );
}

AntennaDetail::AntennaDetail(QString s):PubSubValue(AntennaDetailType)
{
    qRegisterMetaType< AntennaDetail > ( "AntennaDetail" );
    unpack(s);
}
bool AntennaDetail::isDirty() const
{
    return (_minAzimuth.isDirty() || _maxAzimuth.isDirty() || _presets.isDirty());
}
void AntennaDetail::clearDirty()
{
    _minAzimuth.clearDirty();
    _maxAzimuth.clearDirty();
    _presets.clearDirty();
}
void AntennaDetail::setDirty()
{
    _minAzimuth.setDirty();
    _maxAzimuth.setDirty();
    _presets.setDirty();
}
QString AntennaDetail::pack() const
{
    QJsonObject jv;

    jv.insert(rpcConstants::rotatorMinAzimuth, minAzimuth());
    jv.insert(rpcConstants::rotatorMaxAzimuth, maxAzimuth());
    jv.insert(rpcConstants::rotPresetList, presets());

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
        _presets.setValue(json.object().value(rpcConstants::rotPresetList).toString());
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

QString AntennaDetail::presets() const
{
    return _presets.getValue();
}

