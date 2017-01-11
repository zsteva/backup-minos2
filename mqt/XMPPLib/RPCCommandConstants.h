#ifndef RPCCOMMANDRCONSTANTS_H
#define RPCCOMMANDRCONSTANTS_H

namespace rpcConstants
{
// PubSub

static const QString publish            = QT_TR_NOOP("Minos:PubSub:Publish");
static const QString serverSubscribe    = QT_TR_NOOP("Minos:PubSub:ServerSubscribe");
static const QString remoteSubscribe    = QT_TR_NOOP("Minos:PubSub:RemoteSubscribe");
static const QString subscribe          = QT_TR_NOOP("Minos:PubSub:Subscribe");
static const QString serverNotify       = QT_TR_NOOP("Minos:PubSub:ServerNotify");
static const QString clientNotify       = QT_TR_NOOP("Minos:PubSub:ClientNotify");

// Server Categories

static const QString StationCategory    = QT_TR_NOOP("Station");
// station name is the key, IP is the value


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

// Keyer
static const QString keyerApp           = QT_TR_NOOP("Keyer");
static const QString keyerMethod        = QT_TR_NOOP("Minos:KeyerControl");
static const QString keyerPlayFile      = QT_TR_NOOP("PlayFile");
static const QString keyerRecordFile    = QT_TR_NOOP("RecordFile");
static const QString keyerTone          = QT_TR_NOOP("Tone");
static const QString keyerTwoTone       = QT_TR_NOOP("TwoTone");
static const QString keyerStop          = QT_TR_NOOP("Stop");

static const QString KeyerCategory      = QT_TR_NOOP("Keyer");
static const QString keyerKeyReport     = QT_TR_NOOP("Report");

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
static const QString rotatorKeyState    = QT_TR_NOOP("State");
static const QString rotatorBearing     = QT_TR_NOOP("Bearing");

// Rig Control

static const QString RigControlCategory = QT_TR_NOOP("RigControl");
static const QString rigControlKeyMode  = QT_TR_NOOP("Mode");
static const QString rigControlKeyFrequency    = QT_TR_NOOP("Frequency");

// dgb test rpc1

static const QString rpc1App         = QT_TR_NOOP("Rpc1");

// dgb test rpc2

static const QString rpc2App         = QT_TR_NOOP("Rpc2");

}
#endif // RPCCOMMANDRCONSTANTS_H
