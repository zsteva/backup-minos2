#ifndef RIGCONTROLCOMMONCONSTANTS_H
#define RIGCONTROLCOMMONCONSTANTS_H

#include <QStringList>

// Status messages sent to minos logger
#define RIG_STATUS_READY "Ready"
#define RIG_STATUS_DISCONNECTED "Disconnected"
#define RIG_STATUS_ERROR "Error"

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


const QStringList  modeList = {AM, CW, USB, LSB, RTTY, FM, WFM, CWR, RTTYR, AMS,
                          PKTLSB, PKTUSB, PKTFM, ECSSUSB, ECSSLSB, FAX, SAM,
                          SAL, SAH, DSB, FMN };


const QStringList supModeList = { AM, CW, USB, LSB, RTTY, FM, WFM, CWR, RTTYR, PKTLSB, PKTUSB};

}























#endif // RIGCONTROLCOMMONCONSTANTS_H
