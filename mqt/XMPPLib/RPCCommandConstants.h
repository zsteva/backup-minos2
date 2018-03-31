#ifndef RPCCOMMANDRCONSTANTS_H
#define RPCCOMMANDRCONSTANTS_H
#include <QString>

namespace rpcConstants
{
// PubSub

static const QString publish            = QT_TR_NOOP("Minos:PubSub:Publish");
static const QString serverSubscribe    = QT_TR_NOOP("Minos:PubSub:ServerSubscribe");
static const QString remoteSubscribe    = QT_TR_NOOP("Minos:PubSub:RemoteSubscribe");
static const QString subscribe          = QT_TR_NOOP("Minos:PubSub:Subscribe");
static const QString serverNotify       = QT_TR_NOOP("Minos:PubSub:ServerNotify");
static const QString clientNotify       = QT_TR_NOOP("Minos:PubSub:ClientNotify");

static const QString selected           = QT_TR_NOOP("Selected");

// Server Categories

static const QString StationCategory    = QT_TR_NOOP("Station");
static const QString LocalStationCategory    = QT_TR_NOOP("LocalStation");
// station name is the key, IP is the value

// Logger
static const QString loggerApp         = QT_TR_NOOP("Logger");
static const QString LoggerCategory    = QT_TR_NOOP("MinosLogger");

// Monitor
static const QString monitorApp         = QT_TR_NOOP("Monitor");
static const QString monitorLogCategory = QT_TR_NOOP("MonitorLog");
static const QString loggerStanzaRequest = QT_TR_NOOP("Minos:Logger:GetStanza");
static const QString loggerStanzaResponse = QT_TR_NOOP("Minos:Logger:StanzaReturned");

// bandmap
static const QString bandmapApp         = QT_TR_NOOP("BandMap");
static const QString bandmapMethod      = QT_TR_NOOP("Minos:BandMap");

static const QString bandmapParamName   = QT_TR_NOOP("Name");
static const QString bandmapParamFreq   = QT_TR_NOOP("Freq");
static const QString bandmapParamCallsign = QT_TR_NOOP("Callsign");
static const QString bandmapParamLocator= QT_TR_NOOP("Locator");
static const QString bandmapParamUTC    = QT_TR_NOOP("UTC");
static const QString bandmapParamQTH    = QT_TR_NOOP("QTH");

static const QString BandMapCategory    = QT_TR_NOOP("BandMap");
static const QString bandmapKeyLoaded    = QT_TR_NOOP("Loaded");

// Chat
static const QString chatApp            = QT_TR_NOOP("chat");
static const QString chatMethod         = QT_TR_NOOP("Minos:Chat");
static const QString SendChatMessage    = QT_TR_NOOP("SendChatMessage");
static const QString ChatResult         = QT_TR_NOOP("ChatResult");
static const QString ChatCategory       = QT_TR_NOOP("MinosChat");
static const QString ChatServer         = QT_TR_NOOP("MinosChatServer");

// Keyer
static const QString keyerApp           = QT_TR_NOOP("Keyer");
static const QString keyerMethod        = QT_TR_NOOP("Minos:KeyerControl");
static const QString keyerPlayFile      = QT_TR_NOOP("PlayFile");
static const QString keyerRecordFile    = QT_TR_NOOP("RecordFile");
static const QString keyerTone          = QT_TR_NOOP("Tone");
static const QString keyerTwoTone       = QT_TR_NOOP("TwoTone");
static const QString keyerStop          = QT_TR_NOOP("Stop");

static const QString KeyerCategory      = QT_TR_NOOP("Keyer");
static const QString keyerReport     = QT_TR_NOOP("Report");

// Control
static const QString controlApp         = QT_TR_NOOP("Control");

static const QString controlParamName   = QT_TR_NOOP("Name");
static const QString controlParamLine   = QT_TR_NOOP("Line");
static const QString controlResult      = QT_TR_NOOP("ControlResult");
static const QString controlLineState   = QT_TR_NOOP("LineState");
static const QString controlState       = QT_TR_NOOP("State");
static const QString controlGetLine     = QT_TR_NOOP("GetLine");



// Rotator

enum RotateDirection :int {eRotateLeft, eRotateDirect, eRotateRight, eRotateStop};

static const QString rotatorApp         = QT_TR_NOOP("Rotator");
static const QString rotatorMethod      = QT_TR_NOOP("Minos:Rotator");
static const QString rotatorResult      = QT_TR_NOOP("RotatorResult");

static const QString rotatorParamDirection    = QT_TR_NOOP("RotatorDirection");
static const QString rotatorParamAngle  = QT_TR_NOOP("RotatorAngle");

static const QString RotatorCategory    = QT_TR_NOOP("Rotator");
static const QString rotatorDetailCategory    = QT_TR_NOOP("RotatorDetail");
static const QString rotatorStateCategory    = QT_TR_NOOP("RotatorState");
static const QString rotatorState       = QT_TR_NOOP("State");
static const QString rotatorBearing     = QT_TR_NOOP("Bearing");
static const QString rotatorList        = QT_TR_NOOP("rotList");
static const QString rotPreset  = QT_TR_NOOP("RotPreset");
static const QString rotPresetList  = QT_TR_NOOP("RotPresetList");

static const QString rotatorAntennaName = QT_TR_NOOP("AntennaName");
static const QString rotatorMaxAzimuth  = QT_TR_NOOP("MaxAzimuth");
static const QString rotatorMinAzimuth  = QT_TR_NOOP("MinAzimuth");

// Rig Control
static const QString rigControlApp         = QT_TR_NOOP("RigControl");
static const QString rigControlResult      = QT_TR_NOOP("RigControlResult");
static const QString rigControlMethod      = QT_TR_NOOP("Minos:RigControl");
static const QString rigControlCategory    = QT_TR_NOOP("RigControl");
static const QString rigStateCategory      = QT_TR_NOOP("RigState");
static const QString rigDetailsCategory    = QT_TR_NOOP("RigDetails");
static const QString rigControlRadioList   = QT_TR_NOOP("RadioList");
static const QString rigControlBandList    = QT_TR_NOOP("BandList");
static const QString rigControlStatus      = QT_TR_NOOP("State");
static const QString rigControlErrorMsg    = QT_TR_NOOP("ErrorMsg");
static const QString rigControlFreq        = QT_TR_NOOP("Frequency");
static const QString rigControlMode        = QT_TR_NOOP("Mode");
static const QString rigControlRadioName   = QT_TR_NOOP("RadioName");
static const QString rigControlTxVertStatus = QT_TR_NOOP("TransVertStatus");
static const QString rigControlTxVertOffsetFreq = QT_TR_NOOP("TransVertOffSetFreq");
static const QString rigControlTxVertSwitch = QT_TR_NOOP("TransVertSwitch");

}
#endif // RPCCOMMANDRCONSTANTS_H
