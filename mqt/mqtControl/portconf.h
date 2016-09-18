/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#ifndef portconfH
#define portconfH

class commonController;
class PortConfig
{
   public:
      enum PortType {eptWindows, eptSerial, eptK8055, eptUBW};
      QString name;
      PortType portType;
      QString portName;

      PortConfig()
      {}
      PortConfig( const QString &name, PortType portType, const QString &portName ) :
            name( name ), portType( portType ), portName( portName )
      {}
}
;

class LineConfig
{
   public:
      QString name;
      bool lineIn;
      QString portName;
      QString portLineName;

      LineConfig()
      {}
      LineConfig( const QString &name, const QString &lineIn, const QString &portName, const QString &portLineName )
            : name( name ), lineIn( lineIn == "in" ), portName( portName ), portLineName( portLineName )
      {}
}
;

extern bool configurePorts( commonController & );

class TiXmlElement;

class PortConfigure
{
   private:
      std::map <QString, PortConfig> portmap;
      std::map <QString, LineConfig> linemap;

      void SetPorts( TiXmlElement *e );
      void SetLines( TiXmlElement *e );
      void SetEnable( TiXmlElement *e, commonController &monitor );
   public:
      bool configurePorts( commonController & );
};
#endif
