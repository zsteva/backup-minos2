#ifndef RIGCONTROLCOMMONCONSTANTS_H
#define RIGCONTROLCOMMONCONSTANTS_H

#include <QStringList>


// Status messages sent to minos logger
#define RIG_STATUS_CONNECTED "Connected"
#define RIG_STATUS_DISCONNECTED "Disconnected"
#define RIG_STATUS_ERROR "Error"

#define TXVERT_ON "TXVERTON"
#define TXVERT_OFF "TXVERTOFF"


namespace hamlibData
{

// Hamlib Modes

const QString AM = "AM";             // Amplitude Modulation
const QString CW =  "CW";            //  normal sideband
const QString USB = "USB";           // Upper Side Band
const QString LSB = "LSB";           // Lower Side Band
const QString RTTY = "RTTY";         //  Radio Teletype
const QString FM = "FM";             // "narrow" band FM
const QString WFM = "WFM";           // broadcast wide FM
const QString CWR = "CW";            //  "reverse" sideband
const QString RTTYR = "RTTYR";       // RTTY "reverse" sideband
const QString AMS = "AMS";           // Amplitude Modulation Synchronous
const QString PKTLSB = "PKTLSB";     // Packet/Digital LSB mode (dedicated port)
const QString PKTUSB = "PKTUSB";     // Packet/Digital USB mode (dedicated port)
const QString PKTFM = "PKTFM";       // Packet/Digital FM mode (dedicated port)
const QString ECSSUSB = "ECSSUSB";   // Exalted Carrier Single Sideband USB
const QString ECSSLSB = "ECSSLSB";   // Exalted Carrier Single Sideband LSB
const QString FAX = "FAX";           // Facsimile Mode
const QString SAM = "SAM";           // Synchronous AM double sideband
const QString SAL = "SAL";           // Synchronous AM lower sideband
const QString SAH = "SAH";           // Synchronous AM upper (higher) sideband
const QString DSB = "DSB";           // Double sideband suppressed carrier
const QString FMN = "FMN";           // FM Narrow Kenwood ts990s

const QString MGM = "MGM";           // MGM generically




const QStringList  modeList = {AM, CW, USB, LSB, RTTY, FM, WFM, CWR, RTTYR, AMS,
                          PKTLSB, PKTUSB, PKTFM, ECSSUSB, ECSSLSB, FAX, SAM,
                          SAL, SAH, DSB, FMN };


const QStringList supModeList = { CW, USB, FM, MGM};

//enum pBandState {NAR, NOR, WIDE};

//const QStringList pBandStateStr = {"NAR", "NOR", "WIDE"};


const QStringList portTypeList = { "RIG_PORT_NONE", "RIG_PORT_SERIAL", "RIG_PORT_NETWORK", "RIG_PORT_DEVICE",
                                "RIG_PORT_PACKET", "RIG_PORT_DTMF", "RIG_PORT_ULTRA", "RIG_PORT_RPC",
                                "RIG_PORT_PARALLEL, RIG_PORT_USB, RIG_PORT_UDP_NETWORK, RIG_PORT_CM108"};


}




// File Name Constants


const QString LOCAL_RADIO = "Local";

const QString RIG_CONFIGURATION_FILEPATH_LOGGER = "./Configuration/";
const QString RIG_CONFIGURATION_FILEPATH_LOCAL = "./Configuration/";

const QString MINOS_RADIO_CONFIG_FILE = "MinosRigControlConfig.ini";


const QString RADIO_PATH_LOGGER = "./Configuration/Radio/";
const QString RADIO_PATH_LOCAL = "./Configuration/Radio/";

const QString TRANSVERT_PATH_LOGGER = "./Configuration/Radio/Transverter/";
const QString TRANSVERT_PATH_LOCAL = "./Configuration/Radio/Transverter/";

const QString FILENAME_AVAIL_RADIOS = "AvailRadio.ini";
const QString FILENAME_CURRENT_RADIO = "CurrentRadio.ini";
const QString FILENAME_TRANSVERT_RADIOS = "TransVertRadio.ini";




// Tooltips

const QString civToolTip = "Leave field blank for default radio CIV,\nor enter in the form 0xnn where nn is the radio CIV address.";



// RadioName when no radio is connected
const QString NORADIO = "NoRadio";






#endif // RIGCONTROLCOMMONCONSTANTS_H
