#include "RigState.h"
#include "RPCCommandConstants.h"
#include "MTrace.h"

void RigState::setStatus(const QString &status)
{
    _status.setValue(status);
}

RigState::RigState()
    :PubSubValue(RigStateType)
{}
RigState::RigState(const QString &status, const QString &sel, int f, const QString &m, double tvo, int tvsw, bool tvst)
    :PubSubValue(RigStateType)
{
    _status.setValue(status);
    _selected.setValue(sel);
    _freq.setValue(f);
    _mode.setValue(m);
    _transverterOffset.setValue(tvo);
    _transverterSwitch.setValue(tvsw);
    _transverterStatus.setValue(tvst);
}
RigState::RigState(QString s)
    :PubSubValue(RigStateType)
{
    unpack(s);
}
RigState::~RigState()
{

}
bool RigState::isDirty()
{
    return  _status.isDirty() ||
            _selected.isDirty() ||
            _freq.isDirty() ||
            _mode.isDirty() ||
            _transverterOffset.isDirty() ||
            _transverterSwitch.isDirty() ||
            _transverterStatus.isDirty();
}
void RigState::clearDirty()
{
    _status.clearDirty();
    _selected.clearDirty();
    _freq.clearDirty();
    _mode.clearDirty();
    _transverterOffset.clearDirty();
    _transverterSwitch.clearDirty();
    _transverterStatus.clearDirty();
}
void RigState::setSelected(const QString &selected)
{
    _selected.setValue(selected);
}

void RigState::setFreq(double freq)
{
    _freq.setValue(freq);
}

void RigState::setMode(const QString &mode)
{
    _mode.setValue(mode);
}

void RigState::setTransverterOffset(double transverterOffset)
{
    _transverterOffset.setValue(transverterOffset);
}

void RigState::setTransverterSwitch(int transverterSwitch)
{
    _transverterSwitch.setValue(transverterSwitch);
}

void RigState::setTransverterStatus(bool transverterStatus)
{
    _transverterStatus.setValue(transverterStatus);
}

QString RigState::pack()
{
    QJsonObject jv;

    jv.insert(rpcConstants::selected, selected());
    jv.insert(rpcConstants::rigControlState, status());
    jv.insert(rpcConstants::rigControlFreq, freq());
    jv.insert(rpcConstants::rigControlMode, mode());
    jv.insert(rpcConstants::rigControlTxVertOffsetFreq, transverterOffset());
    jv.insert(rpcConstants::rigControlTxVertSwitch, transverterSwitch());
    jv.insert(rpcConstants::rigControlTxVertStatus, transverterStatus());

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
        _selected.setValue(json.object().value(rpcConstants::selected).toString());
        _status.setValue(json.object().value(rpcConstants::rigControlState).toString());
        _freq.setValue(json.object().value(rpcConstants::rigControlFreq).toDouble());
        _mode.setValue(json.object().value(rpcConstants::rigControlMode).toString());
        _transverterOffset.setValue(json.object().value(rpcConstants::rigControlTxVertOffsetFreq).toDouble());
        _transverterSwitch.setValue(json.object().value(rpcConstants::rigControlTxVertSwitch).toInt());
        _transverterStatus.setValue(json.object().value(rpcConstants::rigControlTxVertStatus).toBool());
    }
    else
    {
        trace("Err " + err.errorString() + " Bad Json document " + s);
    }

}
QString RigState::selected() const
{
    return _selected.getValue();
}
QString RigState::status() const
{
    return _status.getValue();
}


double RigState::freq() const
{
    return _freq.getValue();
}

QString RigState::mode() const
{
    return _mode.getValue();
}

double RigState::transverterOffset() const
{
    return _transverterOffset.getValue();
}

int RigState::transverterSwitch() const
{
    return _transverterSwitch.getValue();
}

bool RigState::transverterStatus() const
{
    return _transverterStatus.getValue();
}
