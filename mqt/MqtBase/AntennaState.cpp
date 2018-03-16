#include "AntennaState.h"
#include "RPCCommandConstants.h"
#include "MTrace.h"


void AntennaState::setBearing(int bearing)
{
    _bearing.setValue(bearing);
}

void AntennaState::setState(const QString &state)
{
    _state.setValue(state);
}

void AntennaState::setSelected(const QString &selected)
{
    _selected.setValue(selected);
}

AntennaState::AntennaState(QString s):PubSubValue(AntennaStateType)
{
    unpack(s);
}
AntennaState::~AntennaState()
{
}
bool AntennaState::isDirty()
{
    return (_selected.isDirty() || _state.isDirty() || _bearing.isDirty());
}

QString AntennaState::pack()
{
    QJsonObject jv;

    jv.insert(rpcConstants::selected, selected());
    jv.insert(rpcConstants::rotatorState, state());
    jv.insert(rpcConstants::rotatorBearing, bearing());

    QJsonDocument json(jv);

    QString message(json.toJson(QJsonDocument::Compact));

    return message;
}
void AntennaState::unpack(QString s)
{
    QJsonParseError err;
    QJsonDocument json = QJsonDocument::fromJson(s.toUtf8(), &err);
    if (!err.error)
    {
        _selected.setValue(json.object().value(rpcConstants::selected).toString());
        _bearing.setValue(json.object().value(rpcConstants::rotatorBearing).toInt());
        _state.setValue(json.object().value(rpcConstants::rotatorState).toString());
    }
    else
    {
        trace("Err " + err.errorString() + " Bad Json document " + s);
    }

}
QString AntennaState::state() const
{
    return _state.getValue();
}

int AntennaState::bearing() const
{
    return _bearing.getValue();
}

QString AntennaState::selected() const
{
    return _selected.getValue();
}

