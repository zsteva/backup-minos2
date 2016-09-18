/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#ifndef soundsysH
#define soundsysH

#ifndef MAX_BLOCK_LENGTH
#define MAX_BLOCK_LENGTH 1024
#endif
#ifndef CW_BLOCK_LENGTH
#define CW_BLOCK_LENGTH 256
#endif 
/*
* some good values for block size and count
*/
#define BLOCK_COUNT 20

class WriterThread;
class QtSoundSystem
{
protected:
   int cfgrate;

public:
   volatile int done;
   volatile long samplesremaining;
   volatile long now;
   volatile int sbactive;

   int16_t *dataptr;
   long samples;

   int mset;

   static QtSoundSystem *createSoundSystem();

private:
      // internal values
      WriterThread *wt;
      int sampleRate;
      //====================================================

      int writeAudio( int deadSamples = 0 );

      //====================================================

      volatile long samplesInput;

      void readAudio();

      //====================================================

   public:

      QtSoundSystem();
      virtual ~QtSoundSystem();

      virtual bool initialise( QString &errmess );

      bool startInput( QString fname );
      bool startOutput();
      virtual void terminate();
      virtual int setRate();

      virtual bool startDMA( bool play, const QString &fname );
      virtual void stopDMA();
};

#endif
