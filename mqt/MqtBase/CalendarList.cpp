#include "base_pch.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "Calendar.h"
#include "CalendarList.h"

QMap<QString, QString> contestNameMap;
int calendarFormYear = 0;

QString calendarNameString[] =
    {
        "vhfcontests",
        "hfcontests",
        "microcontests",
        "VHFContestsOther",
        "HFContestsOther",
        "bartgcontests"
    };

//---------------------------------------------------------------------------
QString CalendarYear::getPath()
{
    QString path =  "./Configuration/" + calendarNameString[ type ] + yearString() + ".xml";
    return path;
}
QString CalendarYear::getURL()
{
    QString url = getSite() + calendarNameString[ type ] + yearString() + ".xml";
    return url;

}
//---------------------------------------------------------------------------
QString VHFCalendarYear::getSite()
{
    return "http://www.rsgbcc.org/vhf/";
}
//---------------------------------------------------------------------------
QString HFCalendarYear::getSite()
{
    // Yes, the HF calendar is under the VHF directory!
    return "http://www.rsgbcc.org/vhf/";
}
//---------------------------------------------------------------------------
QString HFBARTGCalendarYear::getSite()
{
    return "http://bartg.rsgbcc.org/";
}
//---------------------------------------------------------------------------
QString MicroCalendarYear::getSite()
{
    return "http://microwave.rsgbcc.org/";
}
//---------------------------------------------------------------------------
QString VHFOtherCalendarYear::getSite()
{
    return "";
}
QString VHFOtherCalendarYear::getPath()
{
    QString p = "./Configuration/" + calendarNameString[ type ] + ".xml";
    return p;
}
QString VHFOtherCalendarYear::getURL()
{
    // No URL
    return QString();
}
//---------------------------------------------------------------------------
QString HFOtherCalendarYear::getSite()
{
    return "";
}
QString HFOtherCalendarYear::getPath()
{
    QString p = "./Configuration/" + calendarNameString[ type ] + ".xml";
    return p;
}
QString HFOtherCalendarYear::getURL()
{
    // No URL
    return QString();
}
//---------------------------------------------------------------------------
QString CTYCalendarYear::getSite()
{
    return "http://www.country-files.com/cty/cty.dat";
}
QString CTYCalendarYear::getPath()
{
    QString p = "./Configuration/cty.dat";
    return p;
}
QString CTYCalendarYear::getURL()
{
    return getSite();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool CalendarYear::downloadFile ( bool showError, QWidget *parent )
{
    QString calendarURL = getURL();


    QNetworkAccessManager m_NetworkMngr;

    QUrl qurl( calendarURL );
    QNetworkRequest qnr( qurl );

    qnr.setRawHeader( "User-Agent" , "Mozilla/4.0 (compatible;Adjsql)" );

    QNetworkReply *reply = m_NetworkMngr.get( qnr );

    QEventLoop loop;
    QObject::connect( reply, SIGNAL( finished() ), &loop, SLOT( quit() ) );
    loop.exec();

    if ( reply->error() == QNetworkReply::NoError )
    {
        int raw = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (raw == 301)
        {
            QUrl redirect =  reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

            QNetworkRequest qnr1( redirect );
            qnr1.setRawHeader( "User-Agent" , "Mozilla/4.0 (compatible;Adjsql)" );

            delete reply;
            reply = m_NetworkMngr.get( qnr1 );
            QEventLoop loop;
            QObject::connect( reply, SIGNAL( finished() ), &loop, SLOT( quit() ) );
            loop.exec();
            raw = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        }
        QByteArray data = reply->readAll();
        if (data.size() > 0)
        {
            QUrl aUrl( calendarURL );
            QFileInfo fileInfo = aUrl.path();

            QFile file( getPath() );
            file.open( QIODevice::WriteOnly );
            file.write( data );
            trace ( "HTPP Get of " + calendarURL + " OK" );
        }
        else
        {
           trace ( "HTPP Get of " + calendarURL + " failed - zero length data returned with attribute " + QString::number(raw));
        }
        delete reply;
        return true;
    }
    else
    {
        trace ( QString( "HTPP Get of " ) + calendarURL + " failed: " + reply->errorString() );
        if ( showError )
        {
            mShowMessage ( QString( "HTPP Get of " ) + calendarURL + " failed: " + reply->errorString(), parent );
        }
    }

    delete reply;
    return false;
}
