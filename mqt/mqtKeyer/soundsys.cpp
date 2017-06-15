/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "base_pch.h"

#include <qendian.h>

#include "keyctrl.h"
#include "keyconf.h"
#include "VKMixer.h"
#include "sbdriver.h"
#include "soundsys.h"
#include "keyers.h"
#include "keyerlog.h"
#include "riff.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wold-style-cast"
// as we don't want to change rtaudio.h...
#include "rtaudio.h"
#pragma GCC diagnostic pop
/*static*/
RtAudioSoundSystem *RtAudioSoundSystem::createSoundSystem()
{
   return new RtAudioSoundSystem();
}
//==============================================================================
RtAudioSoundSystem::RtAudioSoundSystem() :
    sampleRate( 0 )
  , playingFile(false)
  , recordingFile(false)
  , passThrough(false)
  , m_pos(0)
  , p_pos(0)
  , audio(0)
  , inputEnabled(false)
  , outputEnabled(false)
  , passThroughEnabled(false)
{
    try
    {
       audio = new RtAudio();

       // Determine the number of devices available
       unsigned int devices = audio->getDeviceCount();
       // Scan through devices for various capabilities
       RtAudio::DeviceInfo info;
       for ( unsigned int i=0; i<devices; i++ ) {
         info = audio->getDeviceInfo( i );
         if ( info.probed == true ) {
           // Print, for example, the maximum number of output channels for each device
           trace( "device = "  + QString::number(i) +  " " + info.name.c_str());
           trace( "Maximum output channels = " + QString::number(info.outputChannels) + " Maximum input channels = " + QString::number(info.inputChannels));
         }
       }

    }
    catch (RtAudioError &error)
    {
       // Handle the exception here
       trace(error.getMessage().c_str());
       audio = 0;
    }
}
RtAudioSoundSystem::~RtAudioSoundSystem()
{
   passThroughEnabled = false;
   stopDMA();
   try
   {
      // Stop the stream.
      audio->stopStream();
   }
   catch ( RtAudioError& error )
   {
       trace(error.getMessage().c_str());
   }
   delete audio;
}
int RtAudioSoundSystem::setRate(int rate)
{
   sampleRate = rate;
   return sampleRate;
}
int audioCallback( void *outputBuffer, void *inputBuffer,
                                unsigned int nFrames,
                                double streamTime,
                                RtAudioStreamStatus status,
                                void *userData )
{
    RtAudioSoundSystem *qss = static_cast<RtAudioSoundSystem *>(userData);
    return qss->audioCallback(outputBuffer, inputBuffer, nFrames, streamTime, status);
}
int RtAudioSoundSystem::audioCallback( void *outputBuffer, void *inputBuffer,
                                unsigned int nFrames,
                                double /*streamTime*/,
                                RtAudioStreamStatus status )
{
    if (outputBuffer == NULL && inputBuffer == NULL)
    {
        return 0;   // no data
    }

    /*
   To continue normal stream operation, the RtAudioCallback function
   should return a value of zero.  To stop the stream and drain the
   output buffer, the function should return a value of one.  To abort
   the stream immediately, the client should return a value of two.      */

    // Since the number of input and output channels is equal, we can do
    // a simple buffer copy operation here.
    if ( status == RTAUDIO_INPUT_OVERFLOW)
    {
        trace("Stream input underflow detected.");
    }
    if (status == RTAUDIO_OUTPUT_UNDERFLOW)
    {
        trace("Stream output overflow detected.");
    }

    if (outputBuffer != NULL && nFrames > 0)
    {
        memset(outputBuffer, 0, nFrames * 2);
    }
    if (inputBuffer && nFrames)
    {
        const int16_t * q = reinterpret_cast< const int16_t * > ( inputBuffer );
         int16_t maxvol = 0;
         qreal sqaccum = 0.0;

         // determine max for VU meter
        for ( unsigned int i = 0; i < nFrames; i++ )
        {
           int16_t sample = abs( *q++ );
           if ( sample > maxvol )
              maxvol = sample;

           sqaccum += sample * sample;
        }

        qreal rmsval = sqrt(sqaccum/nFrames);
        SoundSystemDriver::getSbDriver() ->WinVUInCallback( maxvol, rmsval, nFrames );   // as we are looking at abs of signed data
    }
    // Need to check and split:

    // Recording - take input and write to file (via buffer and writer stream?)
    // set output buffer to zeroes

    if (inputEnabled && inputBuffer != NULL)
    {
        writeDataToFile(inputBuffer, nFrames);
    }

    // Replay - ignore input, write data to output, morph into piptone

    if (outputEnabled && outputBuffer != NULL)
    {
        readFromFile(outputBuffer, nFrames);
    }
    // Passthrough - copy input to output, morph to piptone at end
    if (passThroughEnabled && outputBuffer != NULL && inputBuffer != NULL)
    {
        memcpy( outputBuffer, inputBuffer, nFrames *  2 );  // frame is sint16, should be single channel
    }

    if (outputBuffer != NULL && nFrames > 0)
    {
        const int16_t * q = reinterpret_cast< const int16_t * > ( outputBuffer );
        int16_t maxvol = 0;
        qreal sqaccum = 0.0;

         // determine max for VU meter
        for ( unsigned int i = 0; i < nFrames; i++ )
        {
           int16_t sample = abs( *q++ );
           if ( sample > maxvol )
              maxvol = sample;

           sqaccum += sample * sample;
        }

        qreal rmsval = sqrt(sqaccum/nFrames);
        SoundSystemDriver::getSbDriver() ->WinVUOutCallback( maxvol, rmsval, nFrames );
    }
    // Normal, no PTT - ignore input, set ouput to zeroes

    return 0;
}

bool RtAudioSoundSystem::initialise( QString &/*errmess*/ )
{
    /*

  struct StreamOptions {
    RtAudioStreamFlags flags;      // A bit-mask of stream flags (RTAUDIO_NONINTERLEAVED, RTAUDIO_MINIMIZE_LATENCY, RTAUDIO_HOG_DEVICE, RTAUDIO_ALSA_USE_DEFAULT)
    unsigned int numberOfBuffers;  // Number of stream buffers.
    std::string streamName;        // A stream name (currently used only in Jack).
    int priority;                  // Scheduling priority of callback thread (only used with flag RTAUDIO_SCHEDULE_REALTIME)

  };

struct StreamParameters {
  unsigned int deviceId;     // Device index (0 to getDeviceCount() - 1).
  unsigned int nChannels;    // Number of channels.
  unsigned int firstChannel; // First channel index on device (default = 0).

};

void openStream( RtAudio::StreamParameters *outputParameters,
               RtAudio::StreamParameters *inputParameters,
               RtAudioFormat format, unsigned int sampleRate,
               unsigned int *bufferFrames, RtAudioCallback callback,
               void *userData = NULL,
               RtAudio::StreamOptions *options = NULL,
               RtAudioErrorCallback errorCallback = NULL
               );
*/
    try
    {
        RtAudio::StreamParameters outParams;
        RtAudio::StreamParameters inParams;
        RtAudio::StreamOptions soptions;

        unsigned int bufferFrames = 256;//0;    // 0 is choose minimum

        outParams.deviceId = 0;
        outParams.firstChannel = 0;
        outParams.nChannels = 1;

        inParams.deviceId = 0;
        inParams.firstChannel = 0;
        inParams.nChannels = 1;

        soptions.flags = 0;
        soptions.numberOfBuffers = 16;//4;
        soptions.priority = 0;
        soptions.streamName = "";

        audio->openStream(&outParams,
                          &inParams,
                          RTAUDIO_SINT16, sampleRate,
                          &bufferFrames, ::audioCallback,
                          static_cast<void *>(this),
                          &soptions
                          );
        trace("Audio stream opened OK");

        audio->startStream();
    }
    catch (RtAudioError &error)
    {
        trace(error.getMessage().c_str());
      // Perhaps try other parameters?
    }

    return true;
}

void RtAudioSoundSystem::startOutput()
{
    trace("startOutput");
    outputEnabled = true;
}

void RtAudioSoundSystem::stopOutput()
{
    outputEnabled = false;
    KeyerAction * sba = KeyerAction::getCurrentAction();
     if ( sba )
     {
        sba->actionTime = 1;    // force to stop
     }
     SoundSystemDriver::getSbDriver() ->WinVUOutCallback( 0, 0, 0 );
}
void RtAudioSoundSystem::startInput()
{
    inputEnabled = true;
}

void RtAudioSoundSystem::stopInput()
{
    inputEnabled = false;
    KeyerAction * sba = KeyerAction::getCurrentAction();
     if ( sba )
     {
        sba->queueFinished();
     }
    outWave.Close();
}
bool RtAudioSoundSystem::startInput( QString fn )
{
    // open fname, assign a text(?)
    // startInput() will also be called later
    if ( outWave.OpenForWrite( fn.toLatin1(), sampleRate, 16, 1 ) == DDC_SUCCESS )
       return true;

    return false;
}

void RtAudioSoundSystem::setData(int16_t *data, int len)
{
    m_buffer.resize(len * sizeof(uint16_t));
    unsigned char *ptr = reinterpret_cast<unsigned char *>(m_buffer.data());

    for (int i = 0; i < len; i++)
    {
        uint16_t value = data[i];
        // This may or may not be neccesary... on a big endian system
        qToLittleEndian<uint16_t>(value, ptr);
        ptr += 2;
    }
    m_pos = 0;
}
void RtAudioSoundSystem::setPipData(int16_t *data, int len, int delayLen)
{
    pipDelayBytes = delayLen * sizeof(uint16_t);
    p_buffer.resize(len * sizeof(uint16_t));
    unsigned char *ptr = reinterpret_cast<unsigned char *>(p_buffer.data());

    for (int i = 0; i < len; i++)
    {
        uint16_t value = data[i];
        // This may or may not be neccesary... on a big endian system
        qToLittleEndian<uint16_t>(value, ptr);
        ptr += 2;
    }
    p_pos = 0;
}

void RtAudioSoundSystem::writeDataToFile(void *inp, int nFrames)
{
    // data arrives here; we need to write it to the (already open) file,


    if (inp && nFrames)
    {
        const int16_t *q = reinterpret_cast< const int16_t * > ( inp );
        DDCRET ret = outWave.WriteData ( q, nFrames );   // size is samples
        if ( ret != DDC_SUCCESS )
        {
            return;
        }
    }
}
void RtAudioSoundSystem::readFromFile(void *outputBuffer, unsigned int nFrames)
{
    if (outputBuffer && nFrames)
    {
        qint64 len = nFrames * 2;
        QByteArray data;

        // we have to add in the pip here as well...
        qint64 total = 0;
        if (m_pos >= m_buffer.size())
        {
            // add in pip delay and pip
            if (p_pos >= p_buffer.size())
            {
                stopOutput();
                return;
            }
            if (pipDelayBytes > 0)
            {
                qint64 ps = pipDelayBytes;
                total = qMin(ps, len);
                data.fill(0, total);
                pipDelayBytes -= total;
            }
            else
            {
                if (!p_buffer.isEmpty())
                {
                    total = qMin((p_buffer.size() - p_pos), len);
                    data.append(p_buffer.constData() + p_pos, total);
                    p_pos += total;
                }
                else
                {
                    p_buffer.clear();
                    stopOutput();
                    trace("p_buffer empty");
                }
            }
        }
        else
        {
            if (!m_buffer.isEmpty())
            {
                total = qMin((m_buffer.size() - m_pos), len);
                data.append(m_buffer.constData() + m_pos, total);

                m_pos += total;
            }
            else
            {
                stopOutput();
                trace("m_buffer empty");
            }
        }
        KeyerAction * sba = KeyerAction::getCurrentAction();
        if ( sba )
           sba->interruptOK();	// so as we do not time it out immediately

        memset(outputBuffer, 0, nFrames * 2);
        memcpy(outputBuffer, data.constData(), total);

    }
}

//==============================================================================
bool RtAudioSoundSystem::startDMA( bool play, const QString &fname )
{
   // start input / output

   if ( play )
   {
        if ( sblog )
        {
            trace( "(StartDMA) Starting output" );
        }

        playingFile = true;
        recordingFile = false;
        passThrough = false;

        setData(dataptr, samples);
        KeyerAction * sba = KeyerAction::getCurrentAction();
        if (sba && sba->tailWithPip)
        {
            long psamples = SoundSystemDriver::getSbDriver() ->pipSamples;
            int16_t *pdataptr = SoundSystemDriver::getSbDriver() ->pipptr;
            setPipData(pdataptr, psamples, sba->pipStartDelaySamples);
        }
        startOutput();
   }
   else
   {
       if ( sblog )
       {
          trace( "(StartDMA) Starting input" );
       }

         if ( !startInput( fname ) )
            return false;

         playingFile = false;
         recordingFile = true;
         passThrough = false;

         startInput();
   }
   return true;
}
void RtAudioSoundSystem::stopDMA()
{
//    Here we need to stop input/output
    trace( "stopDMA" );

    if (playingFile)
        stopOutput();

    if (recordingFile)
        stopInput();

    m_buffer.clear();
    p_buffer.clear();
    m_pos = 0;
    p_pos = 0;

    playingFile = false;
    recordingFile = false;
    passThrough = true;
    SoundSystemDriver::getSbDriver() ->WinVUInCallback( 0, 0, 0 );
    SoundSystemDriver::getSbDriver() ->WinVUOutCallback( 0, 0, 0 );

}
bool RtAudioSoundSystem::startMicPassThrough()
{
    trace("startMicPassThrough");

    passThroughEnabled = true;
   return true;
}
bool RtAudioSoundSystem::stopMicPassThrough()
{
    trace("stopMicPassThrough");

    passThroughEnabled = false;
    return true;
}
