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

#define AM  "AM"            // Amplitude Modulation
#define CW  "CW"            //  "normal" sideband
#define USB "USB"           // Upper Side Band
#define LSB "LSB"           // Lower Side Band
#define RTTY "RTTY"         //  Radio Teletype
#define FM   "FM"           // "narrow" band FM
#define WFM  "WFM"          // broadcast wide FM
#define CWR  "CW"           //  "reverse" sideband
#define RTTYR "RTTYR"       // RTTY "reverse" sideband
#define AMS  "AMS"          // Amplitude Modulation Synchronous
#define PKTLSB "PKTLSB"     // Packet/Digital LSB mode (dedicated port)
#define PKTUSB "PKTUSB"     // Packet/Digital USB mode (dedicated port)
#define PKTFM "PKTFM"       // Packet/Digital FM mode (dedicated port)
#define ECSSUSB "ECSSUSB"   // Exalted Carrier Single Sideband USB
#define ECSSLSB "ECSSLSB"   // Exalted Carrier Single Sideband LSB
#define FAX     "FAX"       // Facsimile Mode
#define SAM "SAM"           // Synchronous AM double sideband
#define SAL "SAL"           // Synchronous AM lower sideband
#define SAH "SAH"           // Synchronous AM upper (higher) sideband
#define DSB "DSB"           // Double sideband suppressed carrier
#define FMN "FMN"           // FM Narrow Kenwood ts990s


QStringList  modeList = { AM, CW, USB, LSB, RTTY, FM, WFM, CWR, RTTYR, AMS,
                          PKTLSB, PKTUSB, PKTFM, ECSSUSB, ECSSLSB, FAX, SAM,
                          SAL, SAH, DSB, FMN };


QStringList supModeList = { AM, CW, USB, LSB, RTTY, FM, WFM, CWR, RTTYR, PKTLSB, PKTUSB};

}























#endif // RIGCONTROLCOMMONCONSTANTS_H
