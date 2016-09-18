/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "base_pch.h"

#include "riff.h"

#include "keyers.h" 
//#include "vkmixer.h"
#include "WriterThread.h"
#include "sbdriver.h"
#include "soundsys.h"


//==============================================================================
int curBlockLength = 0;

//==============================================================================
class dvkFile
{
   public:
      bool loaded;
      bool frec;          // flag set to true if audio has been recorded
      QString fileName;
      long fsample;        // number of bytes for each sound files
      int16_t *fptr;          // data area for each sound file
      unsigned int rate;
      int BitsPerSample;
      int NumChannels;

      bool LoadFile( QString &errmess )
      {
         if ( sblog )
         {
            trace( "Trying to open file " + fileName );
         }
         loaded = false;
         delete[] fptr;
         fptr = 0;
         // should be initiated by keyer, which should call the sound engine
         WaveFile inWave;
         int ret = inWave.OpenForRead( fileName );
         if ( ret != DDC_SUCCESS )
         {
            errmess = "Invalid WAV file " + fileName + "\n";
            if ( sblog )
            {
               trace( errmess );
            }
            return false;
         }
         else
         {
            rate = inWave.SamplingRate();
            BitsPerSample = inWave.BitsPerSample();
            NumChannels = inWave.NumChannels();
            frec = false;
            fsample = inWave.NumSamples();

            if ( rate == 22050 && BitsPerSample == 16 && NumChannels == 1 )
            {
               fptr = new int16_t[ fsample ];
               if ( inWave.ReadData( fptr, fsample ) == DDC_SUCCESS )
               {
                  if ( sblog )
                  {
                     trace( "File " + fileName + " opened samples = " + QString::number( fsample ) );
                  }
                  loaded = true;
               }
               else
               {
                  loaded = false;
               }
            }
            else
            {
               if ( sblog )
               {
                  trace( "File " + fileName + " wrong data format" );
               }
               loaded = false;
            }

         }
         return loaded;
      }
      dvkFile() : loaded( false ), frec( false ), fsample( 0 ), fptr( 0 ), rate( 0 )
      {}
      ~dvkFile()
      {
         delete[] fptr;
         fptr = 0;
      }
};
#define MAXFILES 10
std::vector <dvkFile *> recfil;
//==============================================================================

bool SoundSystemDriver::dofile( int i, int clipRecord )
{
   // mixer set should be set already

   ptr = 0;
   if ( sblog )
   {
      trace( "dofile(" + QString::number( i ) + ")" + "play = " + QString::number( play ) );
   }
   ihand = i;

   // stop the DMA before we start it again
   stopDMA();

   curBlockLength = MAX_BLOCK_LENGTH;

   if ( ihand >= 0 )
   {
      if ( !recording && !recfil[ ihand ] ->loaded )
      {
         return false;
      }
      samples = recfil[ ihand ] ->fsample - clipRecord * ( rate / 1000.0 );
      ptr = recfil[ ihand ] ->fptr;

      if ( sblog )
      {
         trace( "samples = " + QString::number( samples ) );
      }
   }
   else
      if ( play && ihand == DOFILE_PIP )
      {
         // set mic off, wave out on
         ptr = pipptr;
         samples = pipSamples;
         if ( sblog )
         {
            trace( "pipSamples = " + QString::number( samples ) );
         }
      }
      else
         if ( play && ihand == DOFILE_T1 )
         {
            ptr = t1ptr;
            samples = toneSamples;
         }
         else
            if ( play && ihand == DOFILE_T2 )
            {
               ptr = t2ptr;
               samples = toneSamples;
            }
            else
               if ( play && ihand == DOFILE_CW )
               {
                  ptr = cwptr;
                  samples = cwSamples;
               }
   soundSystem->now = 0;
   soundSystem->samplesremaining = samples;
   soundSystem->samples = samples;
   soundSystem->dataptr = ptr;

   soundSystem->done = false;
   soundSystem->sbactive = true;
   /*
    start record/playback!
   */ 
   return soundSystem->startDMA( play, ( ( ihand >= 0 ) ? recfil[ ihand ] ->fileName : QString( "" ) ) );
}


void SoundSystemDriver::stoprec()
{
   if ( sblog )
   {
      trace( "stoprec()" );
   }

   if ( recording )
   {
      if ( currentKeyer )
         currentKeyer->ptt( 0 );
      stopDMA();  // stop - eventually
      CW_ACTIVE = false;
      samples = ( soundSystem->now / 2 < samples ) ? soundSystem->now / 2 : samples;
      ihand = isave;
      recfil[ isave ] ->fsample = samples;
      recording = false;
      //      recfil[isave]->copyfile();
      QString err;
      recfil[ isave ] ->LoadFile( err );
   }
}

void SoundSystemDriver::record_file( const QString &filename )
{
   if ( sblog )
   {
      trace( "record_file(" + filename + ")" );
   }
   stoprec();
   stopDMA();  // stop - eventually
   CW_ACTIVE = false;
   if ( currentKeyer )
      currentKeyer->ptt( 0 );
   unsigned int i;
   for ( i = 0;i < recfil.size();i++ )
   {
      if ( filename.compare(recfil[ i ] ->fileName, Qt::CaseInsensitive ) == 0 )
         break;
   }
   if ( i < recfil.size() )
   {
      delete [] recfil[ i ] ->fptr;  // clear out the data buffer
      recfil[ i ] ->fptr = 0;
   }
   else
   {
      i = recfil.size() + 1;
      if ( i > MAXFILES )
      {
         return ;
      }
      recfil[ i ] ->fileName = filename;
   }

   recfil[ i ] ->loaded = false;
   recfil[ i ] ->rate = rate;
   recfil[ i ] ->frec = true;
   play = false;
   isave = i;
   recording = true;
   dofile( i, 0 );
}
long SoundSystemDriver::play_file( const QString &filename, bool xmit )
{
   if ( sblog )
   {
      trace( "play_file(" + filename + ", " + makeStr( xmit ) + ")" );
   }
   stoprec();
   stopDMA();  // stop - eventually
   CW_ACTIVE = false;
   if ( !xmit && currentKeyer )
      currentKeyer->ptt( 0 );

   if ( filename.compare("AudioCWFile", Qt::CaseInsensitive ) == 0 )
   {
      if ( cwptr && cwSamples )
      {
         if ( xmit && currentKeyer )
         {
            currentKeyer->ptt( 1 );
         }
         play = true;
         recording = false;
         dofile( DOFILE_CW, 0 );
      }
      return cwSamples;
   }
   else
   {
      unsigned int i;
      for ( i = 0;i < recfil.size();i++ )
      {
         if ( filename.compare(recfil[ i ] ->fileName, Qt::CaseInsensitive ) == 0 )
            break;
      }
      if ( i < recfil.size() )
      {
         if ( recfil[ i ] ->loaded && xmit && currentKeyer )
         {
            currentKeyer->ptt( 1 );
         }
         play = true;
         recording = false;
         if ( dofile( i, currentKeyer->kconf.clipRecord ) )
            return recfil[ i ] ->fsample;
      }
      return -1;
   }
}
void SoundSystemDriver::stopall()
{
   ptr = 0;
   stoprec();
   stopDMA();  // stop - eventually
   CW_ACTIVE = false;
   if ( currentKeyer )
      currentKeyer->ptt( 0 );

   delete [] t1ptr;
   delete [] t2ptr;
   delete [] cwptr;
   t1ptr = 0;
   t2ptr = 0;
   toneSamples = 0;
   cwptr = 0;
   cwSamples = 0;
}

bool SoundSystemDriver::sbdvp_init( QString &errmess, int pipTone, int pipVolume, int pipLength )
{
   // should be done from config when the sb is defined as in use.

   // make sure we don't do this twice!
   if ( loadFailed )
   {
      errmess = "Sound card not initialised successfully\n";
      return false;
   }
   if ( !init_done )
   {
      init_done = true;
      loadFailed = true;

      init_OK = true;	// it will have to be undone properly

      // should be in each sound class

      soundSystem->mset = 0; // 16 bit center value

      rate = soundSystem->setRate();
      //    printf("nearest rate is %d Hz\n", rate);

      if ( !soundSystem->initialise( errmess ) )
         return false;

      if ( currentKeyer )
         currentKeyer->ptt( 0 );

      soundSystem->done = true;
      soundSystem->sbactive = false;
      play = true;
      recording = false;

      stopDMA();  // stop - eventually
      CW_ACTIVE = false;

      // clear the recfil structure

      for ( unsigned int i = 0; i < recfil.size(); i++ )
         delete recfil[ i ];
      recfil.clear();
      // attempt to set up all recorded files

      for ( unsigned int fileno = 0;fileno < MAXFILES;fileno++ )
      {
         dvkFile *dvk = new dvkFile;

         char buff[ 100 ];
         sprintf( buff, "CQF%d.WAV", fileno );
         dvk->fileName = buff;

         dvk->LoadFile( errmess );
         recfil.push_back( dvk );
      }
      loadFailed = false;

   }
   createPipTone( errmess, pipTone, pipVolume, pipLength ); // outside conditional to allow for CW calls with -1

   return init_OK;
}


// need to call this from dvpctrl
/*static*/void SoundSystemDriver::sbdvp_unload(    void )
{
   if ( singleton_sb )
   {
      singleton_sb->unload();
      delete singleton_sb;
      singleton_sb = 0;
   }
}
void SoundSystemDriver::unload( void )
{
   if ( sblog )
   {
      trace( "sbDriver::unload()" );
   }
   // should be initiated by keyer, which should call the sound engine
   // when we close everything down (i.e. unregister the monitors, etc.
   if ( init_done && init_OK )
   {

      //disableInterrupts guard;
      init_done = false;
      init_OK = false;

      stopall();
      KeyerAction::currentAction.freeAll();

      //   CLOSE_PCKEYER();
      for ( unsigned int i = 0; i < recfil.size(); i++ )
         delete recfil[ i ];
      recfil.clear();
   }

   delete [] pipptr;
   delete [] t1ptr;
   delete [] t2ptr;
   delete [] cwptr;
   if ( soundSystem )
   {
      soundSystem->terminate();
      delete soundSystem;
      soundSystem = 0;
   }
}
//==============================================================================

void SoundSystemDriver::genTone(int16_t *dest, bool add
                           , int tone, int samples, int rtime, double volmult )
{
   if ( sblog )
   {
      trace( "genTone(" + QString::number( tone ) + ", "
             + QString::number( samples ) + ", "
             + QString::number( rtime ) + ", "
             + QString::number( volmult ) + ")" );
   }

   double deltaAngle = 2 * M_PI * tone / rate;
   double yk = 2 * cos( deltaAngle );
   double y1 = sin ( -2 * deltaAngle );
   double y2 = sin ( -deltaAngle );

#define CHUNKSIZE 1024

   int16_t *buff = new int16_t [ CHUNKSIZE ];

   int16_t *sourceptr = buff;
   int16_t * destptr = dest;

   for ( int buffstart = 0; buffstart < samples; buffstart += CHUNKSIZE )
   {
      int i;
      for ( i = 0; i < CHUNKSIZE && buffstart + i < samples; i++ )
      {
         double y3 = yk * y2 - y1;
         y1 = y2;
         y2 = y3;
         if ( buffstart + i < rtime )
            buff[ i ] = int16_t( y3 * ( ( volmult * ( buffstart + i ) ) / rtime ) );	// not full volume
         else
            if ( buffstart + i > ( samples - rtime ) )
               buff[ i ] = int16_t( y3 * ( ( volmult * ( samples - ( buffstart + i ) ) ) / rtime ) );	// not full volume
            else
               buff[ i ] = int16_t( y3 * volmult );	// not full volume

      }
      // write (or add in place) i bytes to the handle

      if ( add
            )
         {
            for ( int j = 0; j < i; j++ )
            {
               if ( destptr[ j ] + sourceptr[ j ] > 32767 || destptr[ j ] + sourceptr[ j ] < -32768 )
               {
                  mShowMessage( "Big sum...", 0 );
               }
               destptr[ j ] = destptr[ j ] + sourceptr[ j ];
            }
         }
      else
      {
         memcpy( destptr, sourceptr, i * 2 );
      }
      destptr += i;

   }
   delete [] buff;
}
bool SoundSystemDriver::createPipTone( QString &/*errmess*/, int pipTone, int pipVolume, int pipLength )
{
   // should be initiated by keyer, which should call the sound engine
   if ( pipTone > 0 && pipVolume > 0 && pipLength > 0 && ( pipTone != oldpip || pipVolume != oldpipVolume || pipLength != oldpipLength ) )
   {
      oldpip = pipTone;
      oldpipVolume = pipVolume;

      delete [] pipptr;

      pipSamples = rate * ( pipLength / 1000.0 );
      int ramptime = pipSamples / 6;
      pipptr = new int16_t [ pipSamples ];

      const double volmult = 32767.0 * pipVolume / 100.0;
      genTone( pipptr, false, pipTone, pipSamples, ramptime, volmult );
   }
   return true;
}
//==============================================================================
SoundSystemDriver *SoundSystemDriver::singleton_sb = 0;
SoundSystemDriver *SoundSystemDriver::getSbDriver()
{
   if ( singleton_sb )
      return singleton_sb;

   singleton_sb = new SoundSystemDriver();
   singleton_sb->ready = true;      // not until we are out of the constructor
   return singleton_sb;
}
SoundSystemDriver::SoundSystemDriver() :
      CW_ACTIVE( false ), init_done( false ), init_OK( false ), loadFailed( false ),
      pipSamples( 0 ), pipptr( 0 ), oldpip( -1 ), oldpipVolume( -1 ),
      ihand( 0 ), isave( -1 ), recording( false ),
      cwTone( -1 ), ready( false ),
      lastCWRate( 0 ), t1ptr( 0 ), t2ptr( 0 ), toneSamples( 0 ),
      cwptr( 0 ), cwSamples( 0 ),
      soundSystem( 0 ), WinVUInCallback( 0 ), WinVUOutCallback( 0 )
      //,mixer(0)
{
   soundSystem = QtSoundSystem::createSoundSystem();
}
SoundSystemDriver::~SoundSystemDriver()
{
   if ( sblog )
   {
      trace( "sbDriver::~sbDriver" );
   }
   delete soundSystem;
}
//==============================================================================
void SoundSystemDriver::initTone1( int t1 )
{
   delete [] t1ptr;

   toneSamples = rate * ( long ) tuneTime;
   t1ptr = new int16_t [ toneSamples ];
   genTone( t1ptr, false, t1, toneSamples, 5000 /* ramp time*/, ( tuneLevel / 100 ) * 32767.0 );
}
void SoundSystemDriver::initTone2( int t1, int t2 )
{
   delete [] t2ptr;

   toneSamples = rate * ( long ) tuneTime;
   t2ptr = new int16_t [ toneSamples ];
   // ((tuneLevel/100) * 32767.0) / sqrt( 2 ) is an attempt to get equal power - but it can result
   // in peak levels that are out of range, so what to do? Flat top it?
   // Or insist on lower levels?
   genTone( t2ptr, false, t1, toneSamples, 5000 /* ramp time*/, ( ( tuneLevel / 100 ) * 32767.0 ) / 2 );
   genTone( t2ptr, true, t2, toneSamples, 5000 /* ramp time*/, ( ( tuneLevel / 100 ) * 32767.0 ) / 2 );
}
void SoundSystemDriver::startTone1()
{
   play = true;
   dofile( DOFILE_T1, 0 );
}
void SoundSystemDriver::startTone2()
{
   play = true;
   dofile( DOFILE_T2, 0 );
}
// WEIGHT gives the number of elements that make a dit/dah.
// The reminder of 32 (dit) or 64(dah) is the inter-element gap
unsigned char WEIGHT[ 4 ][ 2 ] =
   {{14, 46},           //0.9:1
    {16, 48},           //1.0:1
    {18, 50},           //1.1:1
    {20, 52}};      //1.2:1

// letterspace is 32 (+ prior interelement gap)
// wordspace is 64 (plus prior element/letter gap)

// Using PARIS as standard word, u is unit (dit) length in seconds, c is wpm

// u = c / 1.2

void SoundSystemDriver::createCWBuffer( const char *message, int speed, int tone )
{
   if ( cwptr )
      delete [] cwptr;
   cwSamples = 0;

   int unitlength = rate * ( 1.2 / speed );   // number of samples per unit

   int ditlength = unitlength;
   int dahlength = unitlength * 3;
   int ramptime = unitlength / 8;     // length of ramp on each end of element

   int fulldit = ditlength + unitlength;
   int fulldah = dahlength + unitlength;
   int letterspace = unitlength * 2;
   int wordspace = unitlength * 4;

   int16_t *ditbuff = new int16_t [ fulldit ];
   memset ( ditbuff, 0, fulldit * 2 );
   int16_t *dahbuff = new int16_t [ fulldah * 2 ];
   memset ( dahbuff, 0, fulldah * 2 );

   genTone( ditbuff, false, tone, ditlength, ramptime, 32767.0 * 0.9 );
   genTone( dahbuff, false, tone, dahlength, ramptime, 32767.0 * 0.9 );

   int messlen = strlen( message );
   int messsamples = 0;
   for ( int i = 0; i < messlen; i++ )
   {
      if ( message[ i ] == ' ' )
      {
         messsamples += wordspace;
      }
      else
      {
         try
         {
            QString codestr = MORSECODE[ toupper( message[ i ] ) ];
            for ( int B = 0; B < codestr.size(); B++ )
            {
               if ( codestr[ B ] == '\x40' )
                  messsamples += fulldit;
               else
                  if ( codestr[ B ] == '\x80' )
                     messsamples += fulldah;
            }
            messsamples += letterspace;
         }
         catch ( ... )
         {
            // what should we do if the letter was not found?
         }
      }
   }

   // now we know how large a buffer to allocate

   cwSamples = messsamples;
   cwptr = new int16_t [ messsamples ];
   memset( cwptr, 0, cwSamples * 2 );
   messsamples = 0;
   for ( int i = 0; i < messlen; i++ )
   {
      if ( message[ i ] == ' ' )
      {
         messsamples += wordspace;
      }
      else
      {
         try
         {
            QString codestr = MORSECODE[ toupper( message[ i ] ) ];
            for ( int B = 0; B < codestr.size(); B++ )
            {
               if ( codestr[ B ] == '\x40' )
               {
                  memcpy( &cwptr[ messsamples ], ditbuff, fulldit * 2 );
                  messsamples += fulldit;
               }
               else
                  if ( codestr[ B ] == '\x80' )
                  {
                     memcpy( &cwptr[ messsamples ], dahbuff, fulldah * 2 );
                     messsamples += fulldah;
                  }
            }
            messsamples += letterspace;
         }
         catch ( ... )
         {
            // what should we do if the letter was not found?
         }
      }
   }

   delete [] ditbuff;
   delete [] dahbuff;
   // and now we have a sample filled buffer ready to send
   if ( sblog )
   {
      trace( "cwTone = " + QString::number( tone ) );
      trace( "rate = " + QString::number( speed ) );
      trace( "cwSamples = " + QString::number( cwSamples ) );
      trace( "ramptime = " + QString::number( ramptime ) );
   }
}
//==============================================================================
void SoundSystemDriver::stopDMA()
{
   soundSystem->stopDMA();
}
//==============================================================================
