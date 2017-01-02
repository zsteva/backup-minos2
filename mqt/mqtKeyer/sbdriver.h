/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#ifndef sbdriverH
#define sbdriverH

enum sbControls {ePTT, eL1, eL2};

#define DOFILE_PIP -1
#define DOFILE_T1 -2
#define DOFILE_T2 -3
#define DOFILE_CW -4

class QtSoundSystem;
class SoundSystemDriver
{
   private:
      // another singleton to handle the sb card
      // it may need callbacks to interested parties!
      static SoundSystemDriver *singleton_sb;
      QtSoundSystem *soundSystem;
      //===============================================================
      // working values, read from control on entry, written back on exit

      unsigned long CurrMasterLevel;
      unsigned long CurrRecLevel;
      unsigned long CurrMicLevel;

      //===============================================================

      int oldpip;
      int oldpipVolume;
      int oldpipLength;
      double lastCWRate;

      int ihand, isave;

      void unload( void );
   public:
      volatile int recording;
      bool ready;
      bool loadFailed;

      bool CW_ACTIVE;
      bool init_done;
      bool init_OK;

      int cwTone;

      int pipSamples;
      int16_t * pipptr;

      int toneSamples;
      int16_t * t1ptr;
      int16_t * t2ptr;

      int cwSamples;
      int16_t * cwptr;

      int16_t *ptr;       /* data for current file */
      int samples;   /* fsample for current file  */
      char play;  /* Play or record */

      unsigned int rate;   /* rate in Hertz -- this gets reset to nearest available value */

      VUCallBack WinVUInCallback;
      VUCallBack WinVUOutCallback;

      bool dofile( int i, int clipRecord = 0 );
      void stoprec();
      void record_file( const QString &filename );
      long play_file( const QString &filename, bool xmit );
      void stopall();
      void stopDMA();
      bool rdenv( QString &errmess, QString &in );

      void genTone(int16_t *tptr, bool add
                       , int tone, int samples, int ramptime, double vmult );
      bool createPipTone( QString &errmess, int pipTone, int pipVolume, int pipLength );
      static void sbdvp_unload( void );
      void initTone1( int );
      void initTone2( int, int );
      void startTone1();
      void startTone2();
      void createCWBuffer( const char *message, int speed, int tone );
   public:
      bool sbdvp_init( QString &errmess, int pipTone, int pipVolume, int pipLength );
      SoundSystemDriver();
      ~SoundSystemDriver();

      static SoundSystemDriver *getSbDriver();
};

#endif
