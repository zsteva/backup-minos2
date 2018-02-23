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

const QString FREQ_EDIT_ERR_MSG = QString("Frequency has invalid characters or missing periods.\r\n\r\nThe format required is (e.g.) 1296.3, 144.32 or 144.290123\r\n");

QString convertFreqStrDisp(QString frequency);
QString convertFreqStrDispSingle(QString sfreq);

QString convertFreqToStr(double frequency);

QString convertSinglePeriodFreqToFullDigit(QString f);

double convertStrToFreq(QString frequency);

//QString validateFreqTxtInput(QString f, bool* ok);
bool validateFreqTxtInput(QString f);
QString convertSinglePeriodFreqToMultiPeriod(QString f);

#endif // RIGUTILS_H
