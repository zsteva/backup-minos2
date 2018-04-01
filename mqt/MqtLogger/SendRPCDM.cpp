/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////

#include "logger_pch.h"

#include "ConfigFile.h"

#include "ServerEvent.h"
#include "LogEvents.h"
#include "XMPPRPCParams.h"
#include "XMPPStanzas.h"
#include "Dispatcher.h"
#include "XMPPEvents.h"
#include "XMPPRPCObj.h"
#include "RPCPubSub.h"

#include "SendRPCDM.h"
#include "tsinglelogframe.h"
#include "tlogcontainer.h"
#include "rigutils.h"
//---------------------------------------------------------------------------
TSendDM::TSendDM(QWidget* Owner )
      : QObject( Owner )
{
    MinosRPC *rpc = MinosRPC::getMinosRPC(rpcConstants::loggerApp);
    connect(rpc, SIGNAL(serverCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_serverCall(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(notify(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_notify(bool,QSharedPointer<MinosRPCObj>,QString)));

}
TSendDM::~TSendDM()
{
}
void TSendDM::invalidateCache()
{
    rigCache.invalidate();
    rotatorCache.invalidate();
}
//---------------------------------------------------------------------------
void TSendDM::sendKeyerPlay( TSingleLogFrame *tslf, int fno )
{
   RPCGeneralClient rpc(rpcConstants::keyerMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);
   QSharedPointer<RPCParam>sName(new RPCStringParam( rpcConstants::keyerPlayFile ));
   QSharedPointer<RPCParam>iValue(new RPCIntParam( fno ));
   QSharedPointer<RPCParam>select(new RPCStringParam(tslf->getContest()->uuid ));
   st->addMember( select, rpcConstants::selected );
   st->addMember( sName, "Name" );
   st->addMember( iValue, "Value" );
   rpc.getCallArgs() ->addParam( st );
   rpc.queueCall( tslf->keyerServerConnectable.remoteAppName + "@" + tslf->keyerServerConnectable.serverName );
}
void TSendDM::sendKeyerRecord( TSingleLogFrame *tslf, int fno )
{
   RPCGeneralClient rpc(rpcConstants::keyerMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);
   QSharedPointer<RPCParam>sName(new RPCStringParam( "RecordFile" ));
   QSharedPointer<RPCParam>iValue(new RPCIntParam( fno ));
   QSharedPointer<RPCParam>select(new RPCStringParam(tslf->getContest()->uuid ));
   st->addMember( select, rpcConstants::selected );
   st->addMember( sName, "Name" );
   st->addMember( iValue, "Value" );
   rpc.getCallArgs() ->addParam( st );
   rpc.queueCall( tslf->keyerServerConnectable.remoteAppName + "@" + tslf->keyerServerConnectable.serverName );
}

void TSendDM::sendKeyerTone(TSingleLogFrame *tslf)
{
    RPCGeneralClient rpc(rpcConstants::keyerMethod);
    QSharedPointer<RPCParam>st(new RPCParamStruct);
    QSharedPointer<RPCParam>sName(new RPCStringParam( "Tone" ));
    QSharedPointer<RPCParam>iValue(new RPCIntParam( 0 ));
    QSharedPointer<RPCParam>select(new RPCStringParam(tslf->getContest()->uuid ));
    st->addMember( select, rpcConstants::selected );
    st->addMember( sName, "Name" );
    st->addMember( iValue, "Value" );
    rpc.getCallArgs() ->addParam( st );
    rpc.queueCall( tslf->keyerServerConnectable.remoteAppName + "@" + tslf->keyerServerConnectable.serverName );
}
void TSendDM::sendKeyerTwoTone(TSingleLogFrame *tslf)
{
    RPCGeneralClient rpc(rpcConstants::keyerMethod);
    QSharedPointer<RPCParam>st(new RPCParamStruct);
    QSharedPointer<RPCParam>sName(new RPCStringParam( "TwoTone" ));
    QSharedPointer<RPCParam>iValue(new RPCIntParam( 0 ));
    QSharedPointer<RPCParam>select(new RPCStringParam(tslf->getContest()->uuid ));
    st->addMember( select, rpcConstants::selected );
    st->addMember( sName, "Name" );
    st->addMember( iValue, "Value" );
    rpc.getCallArgs() ->addParam( st );
    rpc.queueCall( tslf->keyerServerConnectable.remoteAppName + "@" + tslf->keyerServerConnectable.serverName );
}
void TSendDM::sendKeyerStop(TSingleLogFrame *tslf)
{
    RPCGeneralClient rpc(rpcConstants::keyerMethod);
    QSharedPointer<RPCParam>st(new RPCParamStruct);
    QSharedPointer<RPCParam>sName(new RPCStringParam( "Stop" ));
    QSharedPointer<RPCParam>iValue(new RPCIntParam( 0 ));
    QSharedPointer<RPCParam>select(new RPCStringParam(tslf->getContest()->uuid ));
    st->addMember( select, rpcConstants::selected );
    st->addMember( sName, "Name" );
    st->addMember( iValue, "Value" );
    rpc.getCallArgs() ->addParam( st );
    rpc.queueCall( tslf->keyerServerConnectable.remoteAppName + "@" + tslf->keyerServerConnectable.serverName );
}
//---------------------------------------------------------------------------
void TSendDM::sendBandMap(  TSingleLogFrame *tslf, const QString &freq, const QString &call, const QString &utc, const QString &loc, const QString &qth )
{
   RPCGeneralClient rpc(rpcConstants::bandmapMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);

   st->addMember( tslf->getContest()->uuid, rpcConstants::selected );
   //st->addMember( rpcConstants::bandmapApp, rpcConstants::bandmapParamName );
   st->addMember( freq, rpcConstants::bandmapParamFreq );
   st->addMember( call, rpcConstants::bandmapParamCallsign );
   st->addMember( loc, rpcConstants::bandmapParamLocator );
   st->addDtgMember( utc, rpcConstants::bandmapParamUTC );
   st->addMember( qth, rpcConstants::bandmapParamQTH );

   rpc.getCallArgs() ->addParam( st );
   rpc.queueCall( tslf->bandMapServerConnectable.remoteAppName + "@" + tslf->bandMapServerConnectable.serverName );
}

void TSendDM::sendRotator(TSingleLogFrame *tslf, rpcConstants::RotateDirection direction, int angle )
{
   RPCGeneralClient rpc(rpcConstants::rotatorMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);

   QSharedPointer<RPCParam>select(new RPCStringParam(tslf->getContest()->uuid ));
   st->addMember( select, rpcConstants::selected );
   st->addMember( static_cast<int> (direction), rpcConstants::rotatorParamDirection );
   st->addMember( angle, rpcConstants::rotatorParamAngle );
   rpc.getCallArgs() ->addParam( st );

   rpc.queueCall( tslf->rotatorServerConnectable.remoteAppName + "@" + tslf->rotatorServerConnectable.serverName );
}
void TSendDM::changeRotatorSelectionTo(const PubSubName &name, const QString &uuid)
{
    // we should de-select the cached uuid on all rotator apps

    PubSubName selected = rotatorCache.getSelected();

    rotatorCache.setSelected(name, uuid);

    if (!selected.isEmpty() && selected != name)
        sendRotatorSelection(selected, "");
    sendRotatorSelection(name, uuid);
}
void TSendDM::sendRotatorSelection(const PubSubName &s, const QString &uuid)
{
    RPCGeneralClient rpc(rpcConstants::rotatorMethod);
    QSharedPointer<RPCParam>st(new RPCParamStruct);

    QSharedPointer<RPCParam>select(new RPCStringParam(uuid ));
    st->addMember( select, rpcConstants::selected );
    st->addMember( s.key(), rpcConstants::rotatorAntennaName );
    rpc.getCallArgs() ->addParam( st );

    rpc.queueCall( s.appName() + "@" + s.server() );
}

void TSendDM::changeRigSelectionTo(const PubSubName &name, const QString &mode, const QString &uuid)
{
    // we should de-select the cached uuid on all rig apps

    PubSubName selected = rigCache.getSelected();

    rigCache.setSelected(name, uuid);

    if (!selected.isEmpty() && selected != name)
        sendRigSelection(selected, "", "");
    sendRigSelection(name, mode, uuid);
}
void TSendDM::sendRigSelection(const PubSubName &s, const QString &mode, const QString &uuid)
{
    RPCGeneralClient rpc(rpcConstants::rigControlMethod);
    QSharedPointer<RPCParam>st(new RPCParamStruct);

    QSharedPointer<RPCParam>select(new RPCStringParam(uuid ));
    st->addMember( select, rpcConstants::selected );
    st->addMember( s.key(), rpcConstants::rigControlRadioName );
    st->addMember( mode, rpcConstants::rigControlMode );
    rpc.getCallArgs() ->addParam( st );

    rpc.queueCall( s.appName() + "@" + s.server() );

}

void TSendDM::sendRigControlFreq(TSingleLogFrame *tslf,const QString &freq)
{
   RPCGeneralClient rpc(rpcConstants::rigControlMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);

   QSharedPointer<RPCParam>select(new RPCStringParam(tslf->getContest()->uuid ));
   st->addMember( select, rpcConstants::selected );
   st->addMember( freq, rpcConstants::rigControlFreq );
   rpc.getCallArgs() ->addParam( st );

   rpc.queueCall( tslf->rigServerConnectable.remoteAppName + "@" + tslf->rigServerConnectable.serverName );
}


void TSendDM::sendRigControlMode(TSingleLogFrame *tslf,const QString &mode)
{
   RPCGeneralClient rpc(rpcConstants::rigControlMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);

   QSharedPointer<RPCParam>select(new RPCStringParam(tslf->getContest()->uuid ));
   st->addMember( select, rpcConstants::selected );
   st->addMember( mode, rpcConstants::rigControlMode );
   rpc.getCallArgs() ->addParam( st );

   rpc.queueCall( tslf->rigServerConnectable.remoteAppName + "@" + tslf->rigServerConnectable.serverName );
}
void TSendDM::sendRotatorPreset(TSingleLogFrame *tslf, QString s)
{
    RPCGeneralClient rpc(rpcConstants::rotatorMethod);
    QSharedPointer<RPCParam>st(new RPCParamStruct);

    st->addMember( s, rpcConstants::rotPreset );
    rpc.getCallArgs() ->addParam( st );

    rpc.queueCall( tslf->rotatorServerConnectable.remoteAppName + "@" + tslf->rotatorServerConnectable.serverName );
}
//---------------------------------------------------------------------------
void TSendDM::on_notify( bool err, QSharedPointer<MinosRPCObj> mro, const QString &from )
{
    // PubSub notifications
    AnalysePubSubNotify an( err, mro );
    trace( "Notify callback from " + from + ( err ? ":Error " : ":Normal " ) +  an.getPublisherProgram() + "@" + an.getPublisherServer());

    if ( an.getOK() /*&& an.getState() == psPublished*/)
    {

        if ( an.getState() == psPublished)
        {
            if ( an.getCategory() == rpcConstants::rigStateCategory)
            {
                 rigCache.setStateString(an);
            }
            if ( an.getCategory() == rpcConstants::rigDetailsCategory)
            {
                 rigCache.setDetailsString(an);
            }
            else if ( an.getCategory() == rpcConstants::rotatorStateCategory )
            {
                rotatorCache.setStateString(an);
            }
            else if ( an.getCategory() == rpcConstants::rotatorDetailCategory )
            {
                rotatorCache.setDetailString(an);
            }
            else if ( an.getCategory() == rpcConstants::rotatorPresetsCategory )
            {
                rotatorCache.setPresetsString(an);
            }
            else if ( an.getCategory() == rpcConstants::rigControlCategory && an.getKey() == rpcConstants::rigControlRadioList )
            {
                rigCache.addRigList(an.getValue());
                emit setRadioLoaded();
                emit setRadioList(an.getValue());
            }
            else if ( an.getCategory() == rpcConstants::RotatorCategory && an.getKey() == rpcConstants::rotatorList )
            {
                rotatorCache.addRotList(an.getValue());
                emit RotatorLoaded();
                emit RotatorList(an.getValue());
            }
        }

        QVector<TSingleLogFrame *> frames = LogContainer->getLogFrames();
        {

            PubSubName rigSelected = rigCache.getSelected();
            if (!rigSelected.isEmpty())
            {
                RigState &selState = rigCache.getState(rigSelected);
                QString selUuid = selState.selected();
                if (!selUuid.isEmpty())
                {
                    for (int i = 0; i < frames.size(); i++)
                    {
                       TSingleLogFrame *tslf = frames[i];
                       QString frameUuid = tslf->getContest()->uuid;

                       if (selUuid == frameUuid)
                       {
                           RigDetails &selDetail = rigCache.getDetails(rigSelected);

                           if (selState.isDirty())
                           {
                               tslf->on_SetMode(selState.mode());
                               tslf->on_SetFreq(convertFreqToStr(selState.freq()));
                               tslf->on_SetRadioState(selState.status());
//                               emit setMode( selState.mode() );
//                               emit setFreq( convertFreqToStr(selState.freq()) );
//                               emit setRadioState( selState.status() );

                               selState.clearDirty();

                           }
                           if (selDetail.isDirty())
                           {
                               tslf->on_SetBandList(selDetail.bandList());
                               tslf->on_SetRadioTxVertState( selDetail.transverterStatus()?"true":"false" );
//                               emit setBandList(selDetail.bandList());
//                               emit setRadioTxVertStatus( selDetail.transverterStatus()?"true":"false" );
                               selDetail.clearDirty();

                           }
                           break;
                       }
                    }
                }
            }
            {
                PubSubName rotSelected = rotatorCache.getSelected();
                if (!rotSelected.isEmpty())
                {
                    AntennaState &selState = rotatorCache.getState(rotSelected);
                    QString selUuid = selState.selected();
                    if (!selUuid.isEmpty())
                    {
                        for (int i = 0; i < frames.size(); i++)
                        {
                           TSingleLogFrame *tslf = frames[i];
                           QString frameUuid = tslf->getContest()->uuid;

                           if (selUuid == frameUuid)
                           {
                               AntennaDetail &selDetail = rotatorCache.getDetails(rotSelected);

                               if (selState.isDirty())
                               {
                                   tslf->on_RotatorBearing(selState.bearing());
                                   tslf->on_RotatorState(selState.state());
                                   //emit RotatorBearing(selState.bearing());
                                   //emit RotatorState(selState.state());
                                   selState.clearDirty();

                               }
                               if (selDetail.isDirty())
                               {
                                   tslf->on_RotatorMaxAzimuth(QString::number(selDetail.maxAzimuth()));
                                   tslf->on_RotatorMinAzimuth(QString::number(selDetail.minAzimuth()));
                                   //emit RotatorMaxAzimuth(QString::number(selDetail.maxAzimuth()));
                                   //emit RotatorMinAzimuth(QString::number(selDetail.minAzimuth()));
                                   selDetail.clearDirty();
                               }
                               if (rotatorCache.rotatorPresetsIsDirty(rotSelected))
                               {
                                    tslf->on_RotatorPresetList(rotatorCache.getRotatorPresets(rotSelected));
                               }
                               break;
                           }
                        }
                    }
                }
            }
        }
        for (int i = 0; i < frames.size(); i++)
        {
           TSingleLogFrame *tslf = frames[i];

           if (an.getPublisherProgram() == tslf->keyerServerConnectable.remoteAppName && an.getPublisherServer() == tslf->keyerServerConnectable.serverName)
            {
                if ( an.getCategory() == rpcConstants::KeyerCategory && an.getKey() == rpcConstants::keyerReport )
                {
                    emit setKeyerLoaded();
                    LogContainer->setCaption( an.getValue() );
                    trace( "KeyerReport " + an.getValue() );
                    break;
                }
            }
            if (an.getPublisherProgram() == tslf->bandMapServerConnectable.remoteAppName && an.getPublisherServer() == tslf->bandMapServerConnectable.serverName)
            {
                if ( an.getCategory() == rpcConstants::BandMapCategory && an.getKey() == rpcConstants::bandmapKeyLoaded )
                {
                    emit setBandMapLoaded();
                    break;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void TSendDM::on_serverCall(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from )
{
    // responds to pull calls from the monitoring client
    trace( "request callback from " + from + ( err ? ":Error" : ":Normal" ) );
    trace("method is " + mro->getMethodName());

    // need to check "from" is correct
    if ( !err )
    {
        QSharedPointer<RPCParam> psLogName;
        QSharedPointer<RPCParam>psStanza;
        RPCArgs *args = mro->getCallArgs();

        QString call = mro->getMethodName();
        if (call == rpcConstants::loggerStanzaRequest)
        {
            if ( args->getStructArgMember( 0, "LogName", psLogName )
                 && args->getStructArgMember( 0, "Stanza", psStanza ) )
            {
                QString LogName;
                int Stanza;
                if ( psLogName->getString( LogName ) && psStanza->getInt( Stanza ) )
                {
                    mro->clearCallArgs();


                    bool callOK = true;
                    // Find Value as a published log
                    // and then send requested stanza from the log
                    // we publish the stanza count; it is up to the monitor to ensure
                    // it has a full set

                    RPCGeneralClient rpc(rpcConstants::loggerStanzaResponse);
                    QSharedPointer<RPCParam>st(new RPCParamStruct);
                    st->addMember( LogName, "LogName" );
                    st->addMember( Stanza, "Stanza" );
                    rpc.getCallArgs() ->addParam( st );

                    // we need to start pushing stanzas from the logfile - we can
                    // only really process a log sequentially
                    // Once it has all been pushed then later stanzas need
                    // to go to all subscribers.

                    // SO it is more in the nature of a "subscribe" but no
                    // get the stanza data from the log and add it as a string
                    TSingleLogFrame * lf = LogContainer ->findContest( LogName );
                    if ( lf )
                    {
                        QString StanzaData;
                        callOK = lf->getStanza( static_cast<unsigned int>(Stanza), StanzaData );
                        if ( callOK )
                        {
                            st->addMember( StanzaData, "StanzaData" );
                        }
                    }
                    else
                    {
                        callOK = false;
                    }

                    st->addMember( callOK, "LoggerResult" );
                    rpc.queueCall( from );

                }
            }
        }
    }
}
//---------------------------------------------------------------------------

