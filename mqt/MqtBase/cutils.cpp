/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "base_pch.h"

bool checkFileOK( std::ifstream &istr, const QString &fname, const QString &fmess )
{
   if ( !istr )
   {
      QString ebuff = QString( "Failed to open %1 (%2)" ).arg(fmess).arg(fname ).arg(strerror( errno));
      MinosParameters::getMinosParameters() ->mshowMessage( ebuff );
      return false;
   }
   return true;
}
char diskBuffer[ bsize + 1 ];
//char *lbuff = &diskBuffer[ 0 ];
size_t buffpt = 0;

void clearBuffer( void )
{
   memset( diskBuffer, '#', bsize );
   diskBuffer[ bsize ] = 0;
}
static const char *noeditstr = "\r\n!!BINARY FILE DO NOT EDIT!!\r\n";
const int noeditlength = 32;

void strtobuf( const QString &str )
{
   buffpt += strcpysp( &diskBuffer[ buffpt ], str, str.length() ) + 1;
}
void strtobuf( const MinosItem<QString> &str )
{
   strtobuf( str.getValue() );
}
void strtobuf()
{
   char * s1 = &diskBuffer[ buffpt ];

   // null fill the rest of the buffer
   memset( s1, 0, bsize - buffpt );
   if ( buffpt + noeditlength < ( bsize - 3 ) )
   {
      memcpy( &diskBuffer[ bsize - noeditlength - 1 ], noeditstr, noeditlength );
   }
   diskBuffer[ bsize - 1 ] = 0;   // force termination
}
void opyn( bool b )
{
   strtobuf( b ? "Y" : "N" );
}
void opyn( const MinosItem<bool> &b )
{
   opyn( b.getValue() );
}

void buftostr( QString &str )
{
   int i;
   str = "";
   size_t len = strlen( &diskBuffer[ buffpt ] );
   for ( i = 0; i < 1024 && i < len; i++ )
   {
      str += diskBuffer[ buffpt + i ];
   }
   buffpt += len + 1;   // step past null terminator
}
void buftostr( MinosItem<QString> &str )
{
   QString temp;
   buftostr( temp );
   str.setInitialValue( temp );
}
bool inyn()
{
   QString temp;
   buftostr( temp );
   if ( temp.size() && temp[ 0 ].toUpper() == 'Y' )
      return true;

   return false;
}
int strcpysp( QString &s1, const QString &s2, int maxlen )
{
    s1 = s2.trimmed().left(maxlen);
    return s1.size();
}
size_t strcpysp( char *s1, const QString &s2, int maxlen )
{
    QString ss2 = s2.trimmed().left(maxlen);

    strcpy(s1, ss2.toLatin1());
    return strlen(s1);
}

//============================================================

int strcmpsp( const QString &s1, const QString &s2 )
{
    QString sp1;
    QString sp2;

   strcpysp( sp1, s1, 255 );
   strcpysp( sp2, s2, 255 );

   return sp1.compare(sp2, Qt::CaseSensitive );
}
//============================================================
int stricmpsp( const QString &s1, const QString &s2 )
{
   QString sp1;
   QString sp2;

   strcpysp( sp1, s1, 255 );
   strcpysp( sp2, s2, 255 );

   return sp1.compare(sp2, Qt::CaseInsensitive );
}
//============================================================

int placestr( QString &buff, const QString &str, int start, int len )
{
   // if len is -ve, it means to R justify

   buff += "     ";
   buff = buff.left(start);
   buff = QString("%1%2                                                                ").arg(buff).arg(str, -len).left(start + abs(len)) ;
   return start + abs(len);
}
//      int scnt = parseLine( buffer, '=', a, 2, 0, sep2seen );
//#warning I want to rewrite parseLine to use strings...

int parseLine( char *buff, char sep, char **a, int count, char sep2, bool &sep2seen )
{
   int i = 0;
   int sep_count = 0;
   sep2seen = false;

   size_t len = strlen( buff );
   for ( int j = 0; j < count; j++ )
   {
      // do it this way so we strip spaces off the start of every element,
      // including the first

      if ( j != 0 )
      {
         // terminate the previous entry on a '<sep>'
         while ( i < len && buff[ i ] && buff[ i ] != sep && buff[ i ] != sep2 )
            i++;

         if ( buff[ i ] == sep || ( sep2 != 0 && buff[ i ] == sep2 ) )
         {
            if ( buff[ i ] == sep2 )
               sep2seen = true;
            sep_count++;
            buff[ i++ ] = 0;
         }
      }
      while ( ( i < len ) && buff[ i ] && ( buff[ i ] == ' ' ) )
         i++;

      a[ j ] = &buff[ i ];
   }
   return sep_count;
}
writer::writer( QSharedPointer<QFile> f ) :  /*lbuff( diskBuffer ),*/ expfd( f )
{}
writer::~writer()
{}
void writer::lwrite( const QString &s )
{
    lwrite(s.toStdString().c_str());
}

void writer::lwrite( const char *b )
{
   QString l = QString( b ) + "\r\n";

   int ret = expfd->write(l.toStdString().c_str(), l.toStdString().size());
   if ( ret != static_cast<int >(l.toStdString().size()) )
   {
      MinosParameters::getMinosParameters() ->mshowMessage( "bad reply from write!" );
   }
}
void writer::lwriteLine()
{
   QString l( 80, 0x5F );					// horizontal line
   lwrite( l );
}
void writer::lwriteNl()
{
    int ret = expfd->write("\r\n", 2);
    if ( ret != 2 )
   {
      MinosParameters::getMinosParameters() ->mshowMessage( "bad reply from write!" );
   }
}
void writer::lwriteFf()
{
   //   ::write( expfd, "\f", 1 );
   int ret = expfd->write("\f", 1);
   if ( ret != 1 )
   {
      MinosParameters::getMinosParameters() ->mshowMessage( "bad reply from write!" );
   }
}
// wild card comparison, search string e for the wild card string in s
// At the moment we are using "space" as the wildcard.
// we always scan down s for the first char in e
bool wildComp( const QString &ss, const QString &ee )
{
   int s = 0;
   int sl = ss.length();
   int e = 0;
   int el = ee.length();

   while (s < sl && ss[s] == ' ' )
      s++;
   if ( s == sl )
      return false;
   while (e < el && ee[e] == ' ' )
      e++;
   if ( e == el )
      return false;


   int estart = e;

   // scan for first char of e in s

   int sstart = s;	// where to restart search

   while ( sstart < sl )
   {
      s = sstart;		// position moving pointer
      e = estart;		// go back to the start of the searching string

      while ( s < sl && e < el && ( ss[s] != ee[e] ) )
         s++;
      if ( s >= sl )
         return false;		// s has ended without a match on char 1 of e

      sstart = ++s;			// next time start one on from this match
      e++;						// first char has matched
      // now attempt to match
      while ( s < sl && e < el )
      {
         if (
            (ss[s] == ee[e] )
            || ( ( ee[e] == ' ' ) || ( ee[e] == '*' ) || ( ee[e] == '?' ) )
         )
         {
            s++;
            e++;
            continue;
         }
         break;		// match failed, break out
      }
      if ( e >= el )
         return true;		// we are at the end of the searching string, so matched

      // otherwise try again at next matching start char
   }
   return false;
}
//=============================================================================
QString trimr( const QString &r )
{
    int n = r.size() - 1;
    for (; n >= 0; --n)
    {
        if (!r.at(n).isSpace())
        {
            return r.left(n + 1);
        }
    }
    return "";
}
//=============================================================================
QString makeADIFField( const QString &fieldname, const QString &content )
{
   QString tag;
   int len = content.size();
   if ( len )
   {
      QString buff = QString( "<%1:%2>" ).arg(fieldname).arg(len );
      tag = buff + content;
   }
   return tag;
}
QString makeADIFField( const QString &fieldname, int content )
{
   return makeADIFField( fieldname, QString::number(content) );
}
//=============================================================================
QString strupr( const QString &s )
{
    return s.toUpper();
}
//=============================================================================
/*
int stricmp( const QString &s1, const QString &s2 )
{
   if ( s2.length() == 0 )
      return -1;
   if ( s1.length() == 0 )
      return 1;
   return s1.compare(s2, Qt::CaseInsensitive );
}
*/
//=============================================================================
int strnicmp( const QString &s1, const QString &s2, unsigned int len )
{
    return s1.left(len).compare(s2.left(len));
}
//=============================================================================
QDateTime CanonicalToTDT(QString cdtg )
{
   QDateTime d;
   d = QDateTime::fromString(cdtg, "yyyyMMddhhmm" );
   d.setTimeSpec(Qt::UTC);
   return d;

}
QString TDTToCanonical(QString d )
{
    // comes in as dd/MM/yyy hh:mm and maybe UTC/GMT
   if (d.endsWith(" UTC"))
        d = d.left(d.length() - 4);
   QDateTime dt = QDateTime::fromString(d, "dd/MM/yyyy hh:mm");
   QString s = dt.toString( "yyyyMMddhhmm" );
   return s;
}
//=============================================================================



int toInt ( const QString &s, int def )
{
    if ( !s.isEmpty() )
    {
        bool ok;
        int i = s.toInt(&ok);
        if (ok)
            return i;
    }
    return def;
}
double toDouble ( const QString &s, double def )
{
    if ( !s.isEmpty() )
    {
        bool ok;
        double d = s.toDouble(&ok);
        if (ok)
            return d;
    }
    return def;
}
QString makeStr( bool i )
{
   return ( i ? "true" : "false" );
}
QString HtmlFontColour( const QColor &c )
{
    QString s = "<font color='" + c.name() + "'>";
    return s;
}
