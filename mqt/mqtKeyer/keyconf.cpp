/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "base_pch.h"

#include "tinyxml.h"
#include "TinyUtils.h"
#include "keyers.h"
#include "portcon.h"
#include "keyconf.h"

//==============================================================================
// The keyer factory
commonKeyer *KeyerConfigure::createKeyer( const KeyerConfig &keyer, const PortConfig &port )
{
   commonKeyer * ck = 0;

   ck = new voiceKeyer( keyer, port );
   if ( sblog )
   {
      trace( "keyer created" );
   }
   if ( !ck->initialise( keyer, port ) )
   {
      delete ck;
      ck = 0;
   }

   return ck;
}
//==============================================================================
//==============================================================================
void KeyerConfigure::SetTrace( TiXmlElement *e )
{
   bool tval;
   if ( GetBoolAttribute( e, "enabled", tval, false ) == TIXML_SUCCESS )
   {
      sblog = tval;
   }
}
void KeyerConfigure::SetTune( TiXmlElement *e )
{
   int tval;
   if ( GetIntAttribute( e, "time", tval, 20 ) == TIXML_SUCCESS )
   {
      if ( tval > 0 && tval <= 30 )
         tuneTime = tval;
      else
      {
         trace( "Incorrect argument to attribute `Tune Time' - must be > 0 and <= 30 (secs)" );
      }
   }
   if ( GetIntAttribute( e, "level", tval, 80 ) == TIXML_SUCCESS )
   {
      if ( tval > 0 && tval <= 100 )
         tuneLevel = tval;
      else
      {
         trace( "Incorrect argument to attribute `Tune Level' - must be > 0 and <= 100 (%)" );
      }
   }
}
void KeyerConfigure::SetPorts( TiXmlElement *e )
{
   for ( TiXmlElement * c = e->FirstChildElement(); c; c = c->NextSiblingElement() )
   {
      if ( checkElementName( c, "Port" ) )
      {
         // attribs name, type, port
         QString n;
         QString t;
         if ( GetStringAttribute( c, "name", n ) == TIXML_SUCCESS
              && GetStringAttribute( c, "type", t ) == TIXML_SUCCESS )
         {
            // should all this live inside PortConfig?
            PortConfig::PortType pt;
            if ( t == "Windows" )
            {
               pt = PortConfig::eptWindows;
            }
            else
               if ( t == "MinosControl" )
               {
                  pt = PortConfig::eptMinosControl;
               }
               else
               {
                  // error...
                  continue;
               }
            PortConfig p( n, pt );
            portmap[ n ] = p;
         }
      }
   }
}
void KeyerConfigure::SetKeyers( TiXmlElement *e )
{

   for ( TiXmlElement * c = e->FirstChildElement(); c; c = c->NextSiblingElement() )
   {
      if ( checkElementName( c, "Keyer" ) )
      {
         // attribs name PipTone EnablePip StartDelay AutoRepeat PipStartDelay PlayPTTDelay
         QString name;
         int PipTone;
         int PipLength;
         bool EnablePip;
         int StartDelay;
         int AutoRepeatDelay;
         bool EnableAutoRepeat;
         int PipStartDelay;
         int PlayPTTDelay;
         int voxHangTime;
         int pipVolume;
         int ClipRecord;

         if ( GetStringAttribute( c, "name", name ) == TIXML_SUCCESS )
         {
            GetIntAttribute( c, "pipTone", PipTone, 1000 );
            GetIntAttribute( c, "pipLength", PipLength, 250 );
            GetBoolAttribute( c, "enablePip", EnablePip, false );
            GetIntAttribute( c, "pipStartDelay", PipStartDelay, 0 );
            GetIntAttribute( c, "pipVolume", pipVolume, 80 );
            GetIntAttribute( c, "startDelay", StartDelay, 0 );
            GetIntAttribute( c, "autoRepeat", AutoRepeatDelay, 6 );
            GetBoolAttribute( c, "enableAutoRepeat", EnableAutoRepeat, false );
            GetIntAttribute( c, "playPTTDelay", PlayPTTDelay, 0 );
            GetIntAttribute( c, "voxHangTime", voxHangTime, 0 );
            GetIntAttribute( c, "clipRecord", ClipRecord, 0 );

            KeyerConfig k( name, PipTone, pipVolume, PipLength, EnablePip, StartDelay,
                           AutoRepeatDelay, EnableAutoRepeat, PipStartDelay, PlayPTTDelay, voxHangTime,
                           ClipRecord );
            keyermap[ name ] = k;
         }
      }
   }
}
void KeyerConfigure::SetEnable( TiXmlElement *e )
{
   //attribs Keyer, Port - refer to names of KeyerConfig, PortConfig
   QString k;
   QString p;
   if ( GetStringAttribute( e, "keyer", k ) == TIXML_SUCCESS
        && GetStringAttribute( e, "port", p ) == TIXML_SUCCESS )
      createKeyer( keyermap[ k ], portmap[ p ] );
}
void KeyerConfigure::SetCW( TiXmlElement *e )
{
   int tone;
   int speed;
   if ( GetIntAttribute( e, "tone", tone, 1000 ) == TIXML_SUCCESS )
   {
      CWTone = tone;
   }
   if ( GetIntAttribute( e, "speed", speed, 12 ) == TIXML_SUCCESS )
   {
      CWSpeed = speed;
   }
}
void KeyerConfigure::SetMorseCode( TiXmlElement *e )
{
   for ( TiXmlElement * c = e->FirstChildElement(); c; c = c->NextSiblingElement() )
   {
      if ( checkElementName( c, "Key" ) )
      {
         QString character;
         QString code;
         if ( GetStringAttribute( c, "character", character ) == TIXML_SUCCESS
              && GetStringAttribute( c, "code", code ) == TIXML_SUCCESS )
         {
            QString proccode;
            for ( int i = 0; i < 8 && code[ i ].unicode() != 0 && code[ i ].unicode() != ' '; i++ )
            {
               if ( code[ i ] == '.' )
                  proccode += '\x40';
               if ( code[ i ] == '-' )
                  proccode += '\x80';
            }
            char c = character[ 0 ].toLatin1();
            MORSECODE[ c ] = proccode;
         }
      }
   }
}
//==============================================================================
/*
-- Variable strings
 
-- $MyCall, $HisCall, $CorrectedCall, $RST, $Serial, $MyLocator, $MyQTH,
-- $CurrentOperator
 
-- are interpreted at transmission time
 
-- Should the callbacks add a space before or after the characters? Ideally not
-- but will this confuse novices.
 
We need a some kind of separator to allow us to parse things at all!
 
-- all messages will be converted to "UPPER CASE" before sending
 
CWMessages = {
	["1"] = "CQ CQ CQ de $MyCall",
	["2"] = "$MyCall",
	["3"] = "$HisCall",
	["4"] = "$CorrectedCall UR $RST $Serial at $MyLocator $MyQTH",
	}
SetMessages(CWMessages)
 
// WGV codings:
 
$1 mycall
$2 "^"
$3 "&"
$4 serial sent
$5 myloc
$6 file contents
$7 his call (autocorrect; only if not as originally sent)
 
// we will use???
 
*/
MORSEMSG::MORSEMSG() : repeat( false )
{}
MORSEMSG::~MORSEMSG()
{}
void MORSEMSG::setMessage( const QString &pvalue )
{
   msg = pvalue;
}
void KeyerConfigure::SetCWMessages( TiXmlElement *e )
{
   for ( TiXmlElement * c = e->FirstChildElement(); c; c = c->NextSiblingElement() )
   {
      if ( checkElementName( c, "Message" ) )
      {
         // Attrib id, autoRepeat; body is message
         // Create a MORSEMSG, and put it into MORSEMSGS
      }
   }
}
bool KeyerConfigure::configureKeyers()
{

   TiXmlBase::SetCondenseWhiteSpace( false );
   TiXmlDocument keyerdoc;
   keyerdoc.LoadFile( "./Configuration/keyerConfig.xml" );
   TiXmlElement *root = keyerdoc.RootElement();
   if ( !root )
   {
      //err = "No XML root element (does file exist?)";
      return false;
   }
   if ( !checkElementName( root, "KeyerConfig" ) )
   {
      //err = "Invalid XML root element";
      return false;
   }
   for ( TiXmlElement * e = root->FirstChildElement(); e; e = e->NextSiblingElement() )
   {
      if ( checkElementName( e, "Trace" ) )
      {
         SetTrace( e );
      }
      else
         if ( checkElementName( e, "Tune" ) )
         {
            SetTune( e );
         }
         else
            if ( checkElementName( e, "Ports" ) )
            {
               SetPorts( e );
            }
            else
               if ( checkElementName( e, "Keyers" ) )
               {
                  SetKeyers( e );
               }
               else
                  if ( checkElementName( e, "Enable" ) )
                  {
                     SetEnable( e );
                  }
                  else
                     if ( checkElementName( e, "CW" ) )
                     {
                        SetCW( e );
                     }
                     else
                        if ( checkElementName( e, "MorseCode" ) )
                        {
                           SetMorseCode( e );
                        }
                        else
                           if ( checkElementName( e, "CWMessages" ) )
                           {
                              SetCWMessages( e );
                           }
                           else
                           {
                              //         trace("Unknown");
                           }
   }
   return true;
}

//==============================================================================
bool configureKeyers()
{
   KeyerConfigure kc;
   return kc.configureKeyers();
}
//==============================================================================
//==============================================================================
