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
//----------------------------------------------------------------------------
extern const double pi /* = (double )3.141592653 */;  /* pi */
extern const double dr /* = pi/180.0*/;      			  // degree to radian conversion factor

extern bool checkFileOK( std::ifstream &istr, const QString &fname, const QString &fmess );

//extern char *lbuff;
extern void clearBuffer( void );
extern void strtobuf( const QString &str );
extern void strtobuf( const MinosItem<QString> &str );
extern void strtobuf();
QString strupr( const QString &s );
extern int stricmp( const QString &s1, const QString &s2 );
extern int strnicmp( const QString &s1, const QString &s2, unsigned int len );
extern void opyn( bool b );
void opyn( const MinosItem<bool> &b );
extern void buftostr( QString &str );
extern void buftostr( MinosItem<QString> &str );
extern bool inyn();
extern bool wildComp( const QString &e, const QString &s );
extern int strcpysp( char *s1, const QString &s2, int maxlen = 255 );
extern int strcpysp( QString &s1, const QString &s2, int maxlen = 255 );
extern int strcmpsp( const QString &s1, const QString &s2 );
extern int stricmpsp( const QString &s1, const QString &s2 );
extern int placestr(QString &buff, const QString &str, int start, int len );
int parseLine( char *buff, char sep, char **a, int count, char sep2, bool &sep2seen );

extern QString trimr( const QString & );

extern QString makeADIFField( const QString &fieldname, const QString &content );
extern QString makeADIFField( const QString &fieldname, int content );
extern QString TDTToCanonical( QString d );
extern QDateTime CanonicalToTDT( QString cdtg );

const int bsize = 256;
extern char diskBuffer[ bsize + 1 ];

extern int buffpt;

class writer
{
      boost::shared_ptr<QFile> expfd;
      //      char *lbuff;

   public:
      void lwrite( const QString & );
      void lwrite( const char * );
      void lwriteLine( void );
      void lwriteNl( void );
      void lwriteFf( void );
      writer( boost::shared_ptr<QFile> f );
      ~writer();
};
bool FileExists( const QString& Name );
bool DirectoryExists(const QString &path );
QString GetCurrentDir();
void SetCurrentDir( const QString &dir );

QString ExtractFileDir( const QString &fname );
QString ExtractFilePath(const QString &fname );
QString ExtractFileName(const QString &fname );
QString ExtractFileExt(const QString &fname );
bool FileAccessible(const QString &fname);
bool FileWriteable(const QString &fname);
bool StaticDirectoryExists ( const QString & Name );
bool StaticForceDirectories (const QString &Dir );
bool StaticDirectoryCreate (const QString & Path , QWidget *Owner);

bool CreateDir( const QString &s );

extern int toInt ( const QString &s, int def = 0 );
extern double toDouble ( const QString &s, double def = 0.0 );
extern QString makeStr( bool i );
extern QString HtmlFontColour( const QColor &c );
#endif
