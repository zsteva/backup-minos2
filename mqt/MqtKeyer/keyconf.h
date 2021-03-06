/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#ifndef keyconfH
#define keyconfH

#include <QString>
#include <QMap>

class PortConfig
{
   public:
      enum PortType {eptWindows, eptMinosControl};
      QString name;
      PortType portType;

      PortConfig()
      {}
      PortConfig( const QString &name, PortType portType ) :
            name( name ), portType( portType )
      {}
}
;

class KeyerConfig
{
   public:
      enum KeyerType {ektVoiceKeyer};
      QString name;
      KeyerType type;
      unsigned int sampleRate;
      int pipTone;
      bool enablePip;
      int startDelay;
      int autoRepeatDelay;
      bool enableAutoRepeat;
      unsigned int pipStartDelay;
      int playPTTDelay;
      int voxHangTime;
      int pipVolume;
      int pipLength;
      int clipRecord;
      int filterCorner;

      KeyerConfig()
      {}
      KeyerConfig( const QString &name, unsigned int rate, int pipTone, int pipVolume, int pipLength, bool enablePip, int startDelay,
                   int autoRepeatDelay, bool enableAutoRepeat, unsigned int pipStartDelay, int playPTTDelay, int voxHangTime, int clipRecord, int filterCorner )
            : name( name ), type( ektVoiceKeyer ), sampleRate(rate), pipTone( pipTone ), enablePip( enablePip ), startDelay( startDelay ),
            autoRepeatDelay( autoRepeatDelay ), enableAutoRepeat( enableAutoRepeat ), pipStartDelay( pipStartDelay ), playPTTDelay( playPTTDelay ),
            voxHangTime( voxHangTime ), pipVolume( pipVolume ), pipLength( pipLength ), clipRecord( clipRecord ), filterCorner(filterCorner)
      {}
}
;

extern bool configureKeyers();

class commonKeyer;
class TiXmlElement;

class KeyerConfigure
{
   private:
      QMap <QString, PortConfig> portmap;
      QMap <QString, KeyerConfig> keyermap;

      commonKeyer *createKeyer( const KeyerConfig &keyer, const PortConfig &port );
      void SetTrace( TiXmlElement *e );
      void SetTune( TiXmlElement *e );
      void SetPorts( TiXmlElement *e );
      void SetKeyers( TiXmlElement *e );
      void SetEnable( TiXmlElement *e );
      void SetCW( TiXmlElement *e );
      void SetMorseCode( TiXmlElement *e );
      void SetCWMessages( TiXmlElement *e );
   public:
      bool configureKeyers();
};
#endif
