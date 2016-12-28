/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
#ifndef CutilsH
#define CutilsH

#include "MinosLoggerEvents.h"

//----------------------------------------------------------------------------
extern const double pi /* = (double )3.141592653 */;  /* pi */
extern const double dr /* = pi/180.0*/;      			  // degree to radian conversion factor

extern bool checkFileOK( std::ifstream &istr, const QString &fname, const QString &fmess );

extern void clearBuffer( void );
extern void strtobuf( const QString &str );
extern void strtobuf( const MinosItem<QString> &str );
extern void strtobuf();
QString strupr( const QString &s );
extern int strnicmp( const QString &s1, const QString &s2, unsigned int len );
extern void opyn( bool b );
void opyn( const MinosItem<bool> &b );
extern void buftostr( QString &str );
extern void buftostr( MinosItem<QString> &str );
extern bool inyn();
extern size_t strcpysp( char *s1, const QString &s2, int maxlen = 255 );
extern int strcpysp( QString &s1, const QString &s2, int maxlen = 255 );
extern int strcmpsp( const QString &s1, const QString &s2 );
extern int stricmpsp( const QString &s1, const QString &s2 );
extern int placestr(QString &buff, const QString &str, int start, int len );
extern int strcspn(const QString &s, const QString &chars);
int parseLine(QString buff, char sep, QStringList &a, int count, char sep2, bool &sep2seen );

extern QString trimr( const QString & );

extern QString makeADIFField( const QString &fieldname, const QString &content );
extern QString makeADIFField( const QString &fieldname, int content );
extern QString TDTToCanonical( QString d );
extern QDateTime CanonicalToTDT( QString cdtg );

const int bsize = 256;
extern char diskBuffer[ bsize + 1 ];

extern size_t buffpt;

class writer
{
      QSharedPointer<QFile> expfd;
      //      char *lbuff;

   public:
      void lwrite( const QString & );
      void lwrite( const char * );
      void lwriteLine( void );
      void lwriteNl( void );
      void lwriteFf( void );
      writer( QSharedPointer<QFile> f );
      ~writer();
};

extern int toInt ( const QString &s, int def = 0 );
extern double toDouble ( const QString &s, double def = 0.0 );
extern QString makeStr( bool i );

// and disallow any other versions
template <class T>
QString makeStr(T) = delete; // C++11

extern QString HtmlFontColour( const QColor &c );

class UpperCaseValidator:public QValidator
{
    bool makeSignal;
public:
    UpperCaseValidator(bool makeSignal = false):makeSignal(makeSignal)
    {

    }

    State validate(QString & input, int & /*pos*/) const
    {
        input = input.toUpper();

        if (makeSignal)
        {
            MinosLoggerEvents::SendValidated();
        }
        return Acceptable;
    }
};
#endif
