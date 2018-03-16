#include "KeyerState.h"
#include "RPCCommandConstants.h"
#include "MTrace.h"

KeyerState::KeyerState(QString s):PubSubValue(KeyerStateType)
{
    unpack(s);
}
KeyerState::~KeyerState()
{

}

QString KeyerState::pack()
{
    QJsonObject jv;

    jv.insert(rpcConstants::selected, _selected);
    jv.insert(rpcConstants::keyerReport, _report);

    QJsonDocument json(jv);

    QString message(json.toJson(QJsonDocument::Compact));

    return message;}
void KeyerState::unpack(QString s)
{
    QJsonParseError err;
    QJsonDocument json = QJsonDocument::fromJson(s.toUtf8(), &err);
    if (!err.error)
    {
        _selected = json.object().value(rpcConstants::selected).toString();
        _report = json.object().value(rpcConstants::keyerReport).toString();
    }
    else
    {
        trace("Err " + err.errorString() + " Bad Json document " + s);
    }

}
QString KeyerState::report() const
{
    return _report;
}

QString KeyerState::selected() const
{
    return _selected;
}
