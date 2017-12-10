/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) D G Balharrie M0DGB/G8FKH
//
/////////////////////////////////////////////////////////////////////////////

#include "rigutils.h"
#include <QRegExp>

// convert freq with delimiter for display

QString convertFreqStrDisp(QString sfreq)
{

    int len = sfreq.length();

    switch(len)
    {
        case 11:
            sfreq = sfreq.insert(8, '.');
            sfreq = sfreq.insert(5, '.');
            sfreq = sfreq.insert(2, '.');
            break;
        case 10:
            sfreq = sfreq.insert(7, '.');
            sfreq = sfreq.insert(4, '.');
            sfreq = sfreq.insert(1, '.');
            break;
        case 9:
            sfreq = sfreq.insert(3, '.');
            sfreq = sfreq.insert(7, '.');
            break;
        case 8:
            sfreq = sfreq.insert(2, '.');
            sfreq = sfreq.insert(6, '.');
            break;
        case 7:
            sfreq = sfreq.insert(4, '.');
            sfreq = sfreq.insert(1, '.');
            break;
        case 6:
            sfreq = sfreq.insert(3,'.');
            break;
        case 5:
            sfreq = sfreq.insert(2,'.');
            break;
        case 4:
            sfreq = sfreq.insert(1,'.');
            break;
        default:
            sfreq = "??.???.???.???";    // error

    }


    return sfreq;
}


QString convertFreqToStr(double frequency)
{

    return QString::number(frequency,'f', 0);

}


double convertStrToFreq(QString frequency)
{
    bool ok = false;
    double f = 0.0;
    f = frequency.toDouble(&ok);
    if (ok)
    {
        return f;
    }
    else
    {
        return -1.0;
    }
}


QString validateFreqTxtInput(QString f, int* ok)
{

    if (f == "")
    {
        *ok = true;
        return f;
    }

    QRegExp re = QRegExp("\\d*");
    QRegExp f1rx = QRegExp("\\d{1,3}\\.\\d{3,3}");          // match mhz.khz
    QRegExp f2rx = QRegExp("\\d{1,2}\\.\\d{3,3}\\.\\d{3,3}"); // match ghz.mhz.khz
    QRegExp f3rx = QRegExp("\\d{1,3}\\.\\d{3,3}\\.\\d{3,3}"); // match mhz.khz.hz
    QRegExp f4rx = QRegExp("\\d{1,2}\\.\\d{3,3}\\.\\d{3,3}\\.\\d{3,3}"); // match ghz.mhz.khz.hz

    QString freq = f;
    int pCount = f.count(QLatin1Char('.'));
    QString rawFreq = f.remove('.');

    QString retFreq = "";

    if (!re.exactMatch(rawFreq) || pCount == 0)
    {
        // not all digits or no periods
        *ok = false;
        return retFreq;
    }

    if (f1rx.exactMatch(freq) || f2rx.exactMatch(freq))
    {
        *ok = true;
        retFreq = freq + ".000";
    }
    else if (f3rx.exactMatch(freq) || f4rx.exactMatch(freq))
    {
        *ok = true;
        retFreq = freq;

    }
    else
    {
        *ok = false;
        retFreq = "error";
    }

    return retFreq;
}
