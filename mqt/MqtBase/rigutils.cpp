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
#include <QStringList>

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


    QRegExp f1rx = QRegExp("\\d{1,5}\\.\\d{3,6}");  // match ghz_mhz.khz_hz

    QString retFreq = "";
    if (f.count('.') != 1)
    {
        *ok = false;
        return f;
    }


    if (f1rx.exactMatch(f))
    {
        QStringList sl = f.split('.');
        if (sl[0].count() > 3)
        {
            retFreq = sl[0].left(sl[0].count()-3) + "." + sl[0].right(3) + ".";
        }
        else
        {
            retFreq = sl[0] + ".";
        }

        if (sl[1].count() > 3)
        {
           retFreq = retFreq + sl[1].left(3) + "." + sl[1].right(sl[1].count()-3);
           if (sl[1].count() == 4)
           {
               retFreq = retFreq + "00";
           }
           else if (sl[1].count() == 5)
           {
               retFreq = retFreq + "0";
           }
        }
        else
        {
           retFreq = retFreq + sl[1] + "." + "000";
        }

        *ok = true;
    }
    else
    {
        // error
        *ok = false;
    }


    return retFreq;
}