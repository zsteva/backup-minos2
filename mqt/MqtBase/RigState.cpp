#include "base_pch.h"
#include "RigState.h"

RigState::RigState()
    :PubSubValue(RigStateType)
{
    qRegisterMetaType< RigState > ( "RigState" );
    _freq.setInitialValue(0.0);
}
RigState::RigState(const QString &status, const QString &sel, int f, const QString &m)
    :PubSubValue(RigStateType)
{
    _status.setValue(status);
    _selected.setValue(sel);
    _freq.setValue(f);
    _mode.setValue(m);
}
RigState::RigState(QString s)
    :PubSubValue(RigStateType)
{
    unpack(s);
}

bool RigState::isDirty() const
{
    return  _status.isDirty() ||
            _selected.isDirty() ||
            _freq.isDirty() ||
            _mode.isDirty();
}
void RigState::clearDirty()
{
    _status.clearDirty();
    _selected.clearDirty();
    _freq.clearDirty();
    _mode.clearDirty();

}
void RigState::setDirty()
{
    _status.setDirty();
    _selected.setDirty();
    _freq.setDirty();
    _mode.setDirty();

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
void RigState::setStatus(const QString &status)
{
    _status.setValue(status);
}

QString RigState::pack() const
{
    QJsonObject jv;

    jv.insert(rpcConstants::selected, selected().getValue());
    jv.insert(rpcConstants::rigControlStatus, status().getValue());
    jv.insert(rpcConstants::rigControlFreq, freq().getValue());
    jv.insert(rpcConstants::rigControlMode, mode().getValue());

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
        _status.setValue(json.object().value(rpcConstants::rigControlStatus).toString());
        _freq.setValue(json.object().value(rpcConstants::rigControlFreq).toDouble());
        _mode.setValue(json.object().value(rpcConstants::rigControlMode).toString());
    }
    else
    {
        trace("Err " + err.errorString() + " Bad Json document " + s);
    }

}
MinosStringItem<QString> RigState::selected() const
{
    return _selected;
}
MinosStringItem<QString> RigState::status() const
{
    return _status;
}


MinosItem<double> RigState::freq() const
{
    return _freq;
}

MinosStringItem<QString> RigState::mode() const
{
    return _mode;
}
