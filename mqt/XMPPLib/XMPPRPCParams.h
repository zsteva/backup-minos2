/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef XMPPRPCParamsH
#define XMPPRPCParamsH

//#include <QMetaType>
//---------------------------------------------------------------------------
// RPC parameter structures

class TiXmlElement;
class RPCParam
{
   public:
      QString name;

      RPCParam();
      virtual ~RPCParam();
      virtual void addParam( TiXmlElement &node );
      virtual void addValue( TiXmlElement &node );
      virtual void addNode( TiXmlElement &node ) = 0;
      virtual QString print() = 0;
      virtual QString analyse() = 0;

      static QSharedPointer<RPCParam> paramFactory( TiXmlElement &node );

      virtual bool getMember( const QString &name, QSharedPointer<RPCParam> &p );
      virtual bool getMember( unsigned int eleno, QSharedPointer<RPCParam> &p );
      virtual bool getElements( unsigned int &size );
      virtual bool getElement( unsigned int eleno, QSharedPointer<RPCParam> &p );
      virtual bool getBoolean( bool &res );
      virtual bool getInt( int &res );
      virtual bool getDouble( double &res );
      virtual bool getString( QString &res );
      virtual bool getBase64( QString &res );
      virtual bool getDtg( QString &res );

      virtual void addMember( QSharedPointer<RPCParam> , const QString &name );
      virtual void addMember( int, const QString &name );
      virtual void addMember( bool, const QString &name );
      virtual void addMember( double, const QString &name );
      virtual void addMember( const QString &, const QString &name );
      virtual void addBase64Member( const QString &, const QString &name );
      virtual void addDtgMember( const QString &, const QString &name );
};

// structure
class RPCParamStruct: public RPCParam
{
      void addMember( RPCParam * , const QString &name );
   protected:
      QVector <QSharedPointer<RPCParam> > elements;
   public:
      RPCParamStruct();
      RPCParamStruct( TiXmlElement &sNode );
      virtual ~RPCParamStruct();

      void addMember( QSharedPointer<RPCParam> , const QString &name );
      void addMember( int, const QString &name );
      void addMember( bool, const QString &name );
      void addMember( double, const QString &name );
      void addMember( const QString &, const QString &name );
      void addBase64Member( const QString &, const QString &name );
      void addDtgMember( const QString &, const QString &name );

      virtual void addNode( TiXmlElement &node );
      virtual QString print();
      virtual QString analyse();

      virtual bool getElements(int &size );
      virtual bool getElement( int eleno, QSharedPointer<RPCParam> &p );
      virtual bool getMember( const QString &name, QSharedPointer<RPCParam> &p );
      virtual bool getMember(int eleno, QSharedPointer<RPCParam> &p );
};

// array
class RPCParamArray: public RPCParam
{
      void addElement( RPCParam * );
   protected:
      QVector <QSharedPointer<RPCParam> > elements;
   public:
      RPCParamArray();
      RPCParamArray( TiXmlElement &sNode );
      virtual ~RPCParamArray();

      void addElement( QSharedPointer<RPCParam>  );

      void addElement( int );
      void addElement( bool );
      void addElement( double );
      void addElement( const char * );
      void AddBase64Element( const char * );
      void AddDtgElement( const char * );
      void addElement( const QString & );
      void AddBase64Element( const QString & );
      void AddDtgElement( const QString & );

      virtual void addNode( TiXmlElement &node );
      static QSharedPointer<RPCParamArray> ParseArray( TiXmlElement &aNode );
      virtual QString print();
      virtual QString analyse();
      virtual bool getElements( int &size );
      virtual bool getElement(int eleno, QSharedPointer<RPCParam> &p );
};

// int or i4
class RPCIntParam: public RPCParam
{
   protected:
      int value;
   public:
      RPCIntParam( int v );
      RPCIntParam( TiXmlElement &sNode );
      RPCIntParam();
      virtual ~RPCIntParam();
      virtual void addNode( TiXmlElement &node );
      virtual QString print();
      virtual QString analyse();

      virtual bool getInt( int &res );
};

// boolean
class RPCBooleanParam: public RPCParam
{
   protected:
      bool value;
   public:
      RPCBooleanParam( bool v );
      RPCBooleanParam( TiXmlElement &sNode );
      RPCBooleanParam();
      virtual ~RPCBooleanParam();
      virtual void addNode( TiXmlElement &node );
      virtual QString print();
      virtual QString analyse();

      virtual bool getBoolean( bool &res );
};

// double
class RPCDoubleParam: public RPCParam
{
   protected:
      double value;
   public:
      RPCDoubleParam( double v );
      RPCDoubleParam( TiXmlElement &sNode );
      RPCDoubleParam();
      virtual ~RPCDoubleParam();
      virtual void addNode( TiXmlElement &node );
      virtual QString print();
      virtual QString analyse();

      virtual bool getDouble( double &res );
};

// string
class RPCStringParam: public RPCParam
{
   protected:
      QString value;
   public:
      RPCStringParam( QString v );
      RPCStringParam( TiXmlElement &sNode );
      RPCStringParam();
      virtual ~RPCStringParam();
      virtual void addNode( TiXmlElement &node );
      virtual QString print();
      virtual QString analyse();

      virtual bool getString( QString &res );
};

// <dateTime.iso8601>
class RPCDtgParam: public RPCParam
{
   protected:
      QString value;   // not we want eventually, but...
   public:
      RPCDtgParam( QString v );
      RPCDtgParam( TiXmlElement &sNode );
      RPCDtgParam();
      virtual ~RPCDtgParam();
      virtual void addNode( TiXmlElement &node );
      virtual QString print();
      virtual QString analyse();

      virtual bool getDtg( QString &res );
};

// base64
class RPCBase64Param: public RPCParam
{
   protected:
      QString value;
   public:
      RPCBase64Param( QString v );
      RPCBase64Param( TiXmlElement &sNode );
      RPCBase64Param();
      virtual ~RPCBase64Param();
      virtual void addNode( TiXmlElement &node );
      virtual QString print();
      virtual QString analyse();

      virtual bool getBase64( QString &res );
};

class RPCArgs
{
      void addParam( RPCParam *  );
   public:
      RPCArgs();
      virtual ~RPCArgs();

      virtual QString PrintArgs();

      QVector <QSharedPointer<RPCParam> > args;
      TIXML_STRING UTF8XML; // pre-built params structure

      void addParam( QSharedPointer<RPCParam> );
      void addParam( int );
      void addParam( bool );
      void addParam( double );
      void addParam( const QString & );
      void addBase64Param( const QString & );
      void addDtgParam( const QString & );

      TiXmlElement * makeParamsNode(  );
      QString makeParamsString();
      void addParams( TiXmlElement &paramsNode );       // add args to the node

      bool parseParams( TIXML_STRING ); // parse from the string to args

      bool getStructArgMember(int argno, const QString &name, QSharedPointer<RPCParam>  &res );
      bool getStructArgMember( int argno, unsigned int eleno, QSharedPointer<RPCParam>  &res );
      bool getArrayArgElements(int argno, unsigned int &size );
      bool getArrayArgElement( int argno, unsigned int eleno, QSharedPointer<RPCParam> &res );

      bool getBooleanArg(int argno, bool &res );
      bool getIntArg( int argno, int &res );
      bool getDoubleArg(int argno, double &res );
      bool getStringArg( int argno, QString &res );
      bool getBase64Arg(int argno, QString &res );
      bool getDtgArg( int argno, QString &res );
};
Q_DECLARE_METATYPE(RPCArgs);
//---------------------------------------------------------------------------
#endif
