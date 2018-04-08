#include "base_pch.h"
#include "AntennaState.h"

AntennaState::AntennaState():PubSubValue(AntennaStateType)
{
}
AntennaState::AntennaState(const QString &st, const QString &sel, const QString &b):PubSubValue(AntennaStateType)
{
    _bearing.setValue(b);
    _state.setValue(st);
    _selected.setValue(sel);
}
AntennaState::AntennaState(QString s):PubSubValue(AntennaStateType)
{
    qRegisterMetaType< AntennaState > ( "AntennaState" );
    unpack(s);
}
bool AntennaState::isDirty() const
{
    return (_selected.isDirty() || _state.isDirty() || _bearing.isDirty());
}
void AntennaState::clearDirty()
{
    _selected.clearDirty();
    _state.clearDirty();
    _bearing.clearDirty();
}
void AntennaState::setDirty()
{
    _selected.setDirty();
    _state.setDirty();
    _bearing.setDirty();
}

QString AntennaState::pack() const
{
    QJsonObject jv;

    jv.insert(rpcConstants::selected, selected().getValue());
    jv.insert(rpcConstants::rotatorState, state().getValue());
    jv.insert(rpcConstants::rotatorBearing, bearing().getValue());

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
        _bearing.setValue(json.object().value(rpcConstants::rotatorBearing).toString());
        _state.setValue(json.object().value(rpcConstants::rotatorState).toString());
    }
    else
    {
        trace("Err " + err.errorString() + " Bad Json document " + s);
    }

}
void AntennaState::setBearing(const QString &bearing)
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

MinosStringItem<QString> AntennaState::state() const
{
    return _state;
}

MinosStringItem<QString> AntennaState::bearing() const
{
    return _bearing;
}

MinosStringItem<QString> AntennaState::selected() const
{
    return _selected;
}

