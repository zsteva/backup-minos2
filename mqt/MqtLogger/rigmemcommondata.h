#ifndef RIGMEMCOMMONDATA_H
#define RIGMEMCOMMONDATA_H


namespace maskData {

const QStringList freqMask = { "9.999",
                         "99.999",
                         "999.999",
                         "9.999.999",
                         "99.999.999",
                         "999.999.999",
                         "9.999.999.999",
                         "99.999.999.999"};
enum freqMask_lookup {
  KHZ = 0,              /* 9 Khz   */
  KHZ99,        		/* 99 khz  */
  KHZ999,        		/* 999 khz */
  MHZ,                  /* 9 Mhz   */
  MHZ99,        		/* 99 Mhz  */
  MHZ999,       		/* 999 Mhz */
  GHZ,                  /* Ghz     */
  GHZ99                 /* Ghz 99  */
};


}

namespace bandSelData {

const QStringList bandNames = { "",
                                "10m",
                                "6m",
                                "4m",
                                "2m",
                                "70cm",
                                "23cm",
                                "GHz"};

const QStringList bandFreq = { "",
                               "28.170.000",
                               "50.170.000",
                               "70.210.000",
                               "144.290.000",
                               "432.170.000",
                               "1.296.210.000",
                               "10.000.000.000"};

const QStringList freqDialZero =  { "",
                                    "00.000.000",
                                    "00.000.000",
                                    "00.000.000",
                                    "000.000.000",
                                    "000.000.000",
                                    "0.000.000.000",
                                    "00.000.000.000"};

const maskData::freqMask_lookup bandMaskIdx[] = { maskData::MHZ99,      /* Not used */
                                                    maskData::MHZ99,    /* 10m  */
                                                    maskData::MHZ99,    /* 6m  */
                                                    maskData::MHZ99,    /* 4m  */
                                                    maskData::MHZ999,   /* 2m  */
                                                    maskData::MHZ999,   /* 70cm  */
                                                    maskData::GHZ,      /* 23cm  */
                                                    maskData::GHZ99};   /* GHz  */


}

namespace memoryData {

const int  NUM_MEMORIES = 10;


struct  memData {
    QString callsign;
    QString freq;
    QString locator;
    int bearing;
    QString time;
};




const QStringList memoryTitle = { "M1:",
                                  "M2:",
                                  "M3:",
                                  "M4:",
                                  "M5:",
                                  "M6:",
                                  "M7:",
                                  "M8:",
                                  "M9:",
                                  "M10:"};

const QString memTitleBlank = "        ";


}




















#endif // RIGMEMCOMMONDATA_H
