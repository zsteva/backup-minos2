#include "RigState.h"
#include "RPCCommandConstants.h"
#include "MTrace.h"

RigState::RigState(QString s):PubSubValue(RigStateType)
{
    unpack(s);
}
RigState::~RigState()
{

}

QString RigState::pack()
{
    QJsonObject jv;

    jv.insert(rpcConstants::selected, _selected);
    jv.insert(rpcConstants::rigControlFreq, _freq);
    jv.insert(rpcConstants::rigControlMode, _mode);
    jv.insert(rpcConstants::rigControlTxVertOffsetFreq, _transverterOffset);
    jv.insert(rpcConstants::rigControlTxVertSwitch, _transverterSwitch);
    jv.insert(rpcConstants::rigControlTxVertStatus, _transverterStatus);

    QJsonDocument json(jv);

    QString message(json.toJson(QJsonDocument::Compact));

    return message;
}
void RigState::unpack(QString s)
{
    QJsonParseError err;
    QJsonDocument json = QJsonDocument::fromJson(s.toUtf8(), &err);
    if (!err.error)
    {
        _selected = json.object().value(rpcConstants::selected).toString();
        _freq = json.object().value(rpcConstants::rigControlFreq).toDouble();
        _mode = json.object().value(rpcConstants::rigControlMode).toString();
        _transverterOffset = json.object().value(rpcConstants::rigControlTxVertOffsetFreq).toDouble();
        _transverterSwitch = json.object().value(rpcConstants::rigControlTxVertSwitch).toInt();
        _transverterStatus = json.object().value(rpcConstants::rigControlTxVertStatus).toString();
    }
    else
    {
        trace("Err " + err.errorString() + " Bad Json document " + s);
    }

}
QString RigState::selected() const
{
    return _selected;
}

double RigState::freq() const
{
    return _freq;
}

QString RigState::mode() const
{
    return _mode;
}

double RigState::transverterOffset() const
{
    return _transverterOffset;
}

int RigState::transverterSwitch() const
{
    return _transverterSwitch;
}

QString RigState::transverterStatus() const
{
    return _transverterStatus;
}
