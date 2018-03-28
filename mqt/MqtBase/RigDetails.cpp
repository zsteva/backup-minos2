#include "RigDetails.h"
#include "RPCCommandConstants.h"
#include "MTrace.h"


RigDetails::RigDetails()
    :PubSubValue(RigDetailsType)
{
    qRegisterMetaType< RigDetails > ( "RigDetails" );

}
RigDetails::RigDetails(double tvo, int tvsw, bool tvst, QString blist)
    :PubSubValue(RigDetailsType)
{
    _transverterOffset.setValue(tvo);
    _transverterSwitch.setValue(tvsw);
    _transverterStatus.setValue(tvst);
    _bandList.setValue(blist);
}
RigDetails::RigDetails(QString s)
    :PubSubValue(RigDetailsType)
{
    unpack(s);
}

bool RigDetails::isDirty() const
{
    return
            _transverterOffset.isDirty() ||
            _transverterSwitch.isDirty() ||
            _transverterStatus.isDirty() ||
            _bandList.isDirty();
}
void RigDetails::clearDirty()
{
    _transverterOffset.clearDirty();
    _transverterSwitch.clearDirty();
    _transverterStatus.clearDirty();
    _bandList.clearDirty();
}
void RigDetails::setDirty()
{
    _transverterOffset.setDirty();
    _transverterSwitch.setDirty();
    _transverterStatus.setDirty();
    _bandList.setDirty();
}


void RigDetails::setTransverterOffset(double transverterOffset)
{
    _transverterOffset.setValue(transverterOffset);
}

void RigDetails::setTransverterSwitch(int transverterSwitch)
{
    _transverterSwitch.setValue(transverterSwitch);
}

void RigDetails::setTransverterStatus(bool transverterStatus)
{
    _transverterStatus.setValue(transverterStatus);
}
void RigDetails::setBandList(const QString &bandList)
{
    _bandList.setValue( bandList);
}

QString RigDetails::pack() const
{
    QJsonObject jv;

    jv.insert(rpcConstants::rigControlTxVertOffsetFreq, transverterOffset());
    jv.insert(rpcConstants::rigControlTxVertSwitch, transverterSwitch());
    jv.insert(rpcConstants::rigControlTxVertStatus, transverterStatus());
    jv.insert(rpcConstants::rigControlBandList, bandList());

    QJsonDocument json(jv);

    QString message(json.toJson(QJsonDocument::Compact));

    return message;
}
void RigDetails::unpack(QString s)
{
    QJsonParseError err;
    QJsonDocument json = QJsonDocument::fromJson(s.toUtf8(), &err);
    if (!err.error)
    {
        _transverterOffset.setValue(json.object().value(rpcConstants::rigControlTxVertOffsetFreq).toDouble());
        _transverterSwitch.setValue(json.object().value(rpcConstants::rigControlTxVertSwitch).toInt());
        _transverterStatus.setValue(json.object().value(rpcConstants::rigControlTxVertStatus).toBool());
        _bandList.setValue(json.object().value(rpcConstants::rigControlBandList).toString());
    }
    else
    {
        trace("Err " + err.errorString() + " Bad Json document " + s);
    }

}
double RigDetails::transverterOffset() const
{
    return _transverterOffset.getValue();
}

int RigDetails::transverterSwitch() const
{
    return _transverterSwitch.getValue();
}

bool RigDetails::transverterStatus() const
{
    return _transverterStatus.getValue();
}
QString RigDetails::bandList() const
{
    return _bandList.getValue();
}
