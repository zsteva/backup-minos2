/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) D G Balharrie M0DGB/G8FKH
//
/////////////////////////////////////////////////////////////////////////////

#ifndef RIGUTILS_H
#define RIGUTILS_H

#include <QString>

QString convertFreqStrDisp(QString frequency);
QString convertFreqStrDispSingle(QString sfreq);

QString convertFreqToStr(double frequency);

double convertStrToFreq(QString frequency);

//QString validateFreqTxtInput(QString f, bool* ok);
bool validateFreqTxtInput(QString f);
QString convertSinglePeriodFreqToMultiPeriod(QString f);

#endif // RIGUTILS_H
