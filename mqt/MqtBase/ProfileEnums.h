/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
#ifndef ProfileEnumsH
#define ProfileEnumsH
//----------------------------------------------------------------------------
enum LOGGERPROFILE {
   elpPreloadFile, elpPreloadSection,
   elpListsFile, elpListsSection,
   elpDisplayFile, elpDisplaySection,
   elpOperatorFile, elpOperatorSection,
   elpListDirectory, elpLogDirectory,
   elpLocsFile,
   elpEntryFile,
   elpStationFile,
   elpQTHFile,
   elpAutoFill,
   elpAuxWindows
};
enum PRELOADPROFILE {eppSession, eppCurrent, eppDefSession};

enum DISPLAYPROFILE {edpShowContinentEU, edpShowContinentAS,
                     edpShowContinentAF, edpShowContinentOC,
                     edpShowContinentSA, edpShowContinentNA,
                     edpShowWorked, edpShowUnworked,
                     edpStackFrame,
                     edpNextContactDetailsOnLeft,
                     edpShowOperators,
                     edpEditor, edpStatisticsPeriod1, edpStatisticsPeriod2
                    };
enum ENTRYPROFILE {eepCall, eepEntrant, eepMyName, eepMyCall,
                   eepMyAddress1, eepMyAddress2, eepMyCity, eepMyPostCode, eepMyCountry,
                   eepMyPhone, eepMyEmail};
enum QTHPROFILE {eqpLocator, eqpDistrict, eqpLocation, eqpStationQTH1, eqpStationQTH2, eqpASL};
enum STATIONPROFILE {espPower, espTransmitter, espReceiver, espAntenna, espAGL, espOffset, espRadioName, espRotatorName};

enum PROFILES {epLOGGERPROFILE, epPRELOADPROFILE, epLISTSPROFILE, epDISPLAYPROFILE,
               epENTRYPROFILE, epQTHPROFILE, epSTATIONPROFILE, epLOCSQUARESPROFILE,
               epMAXPROFILE};
#endif
