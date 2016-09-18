/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "base_pch.h"

#include <process.h>
#include "keyctrl.h"
#include "keyconf.h"
#include "vkmixer.h"
#include "sbdriver.h"
#include "soundsys.h"
#include "keyers.h"
#include "writerthread.h"
#include "keyerlog.h"

extern int curBlockLength;
// hope this is the one just output...
#define pttype INT16
// VU meter takes 65536 as 100%
#define shortmult 2
//==============================================================================
//static CRITICAL_SECTION waveCriticalSection;
//==============================================================================

/*static*/
QtSoundSystem *QtSoundSystem::createSoundSystem()
{
   return new QtSoundSystem();
}
//==============================================================================
QtSoundSystem::QtSoundSystem() :
    sampleRate( 0 ), wt(0)
{
}
QtSoundSystem::~QtSoundSystem()
{
   stopDMA();
}
int QtSoundSystem::setRate()
{
   int rate = cfgrate;
   if ( rate < 1 )                   // as we have no way of setting it this is always true...
      rate = 22050;
   rate = ( rate > 44100 ) ? 44100 : rate;
   rate = ( rate < 8000 ) ? 8000 : rate;

   sampleRate = rate;
   return rate;

}

bool QtSoundSystem::startInput( QString fname )
{
   return false;
}
bool QtSoundSystem::startOutput()
{
   return false;
}
bool QtSoundSystem::initialise( QString &/*errmess*/ )
{
   return true;
}
void QtSoundSystem::terminate()
{

}
//=============================================================================
void QtSoundSystem::readAudio()
{
}
//=============================================================================
int QtSoundSystem::writeAudio( int deadSamples )
{
    // deadSamples originally used in delaying the start of pip
   return 0;
}
bool QtSoundSystem::startDMA( bool play, const QString &fname )
{
   // start input / output

   if ( play )
   {
      if ( sblog )
      {
         trace( "Starting output" );
      }
      if ( !startOutput() )
      {
         if ( sblog )
         {
            trace( "Start output failed" );
         }
         return false;
      }
      if ( sblog )
      {
         trace( "Start output OK" );
      }
      // and now we have to start writing the file to the wave
      // buffers

      for ( int i = 0; i < BLOCK_COUNT; i++ )
         writeAudio();
      // and the output thread should pick it up from here...
      if ( sblog )
      {
         trace( "All buffers written" );
      }
   }
   else
   {

      if ( !startInput( fname ) )
         return false;
      // and now we have to start giving buffers to the system
      samplesInput = 0;


     if ( sblog )
     {
        trace( "waveInStart failed " );
     }
     return false;
   }
   return true;
}
void QtSoundSystem::stopDMA()
{
   trace( "QtSoundSystem::stopDMA" );
}
