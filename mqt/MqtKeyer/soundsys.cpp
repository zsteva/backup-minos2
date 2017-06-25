/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "base_pch.h"

#include <QtEndian>
#include <QtMath>
#include <numeric>
#include <QtCore>

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
#include "RtAudio.h"
#pragma GCC diagnostic pop

#include "SimpleComp.h"

#define FRAMES 16
#define FRAMESAMPLES 256
#define RINGBUFFERSIZE 1024

QWaitCondition bufferNotEmpty;
QWaitCondition bufferNotFull;
QMutex mutex;

class InBuff
{
public:
    int frameCount;
    int16_t buff[FRAMESAMPLES * 2];
};

InBuff inBuffs[RINGBUFFERSIZE];
int recIndex = 0;
int writeIndex = 0;

RiffWriter::RiffWriter(RtAudioSoundSystem *parent) : QThread(parent), ss(parent), terminated(false)
{
}
RiffWriter::~RiffWriter(){}

void RiffWriter::run()
{
    for (;;)
    {
        mutex.lock();
        if (writeIndex == recIndex)
            bufferNotEmpty.wait(&mutex);
        mutex.unlock();

        if (terminated)
            break;

        if (inBuffs[writeIndex%RINGBUFFERSIZE].frameCount > 0)
            ss->writeDataToFile(inBuffs[writeIndex%RINGBUFFERSIZE].buff, inBuffs[writeIndex%RINGBUFFERSIZE].frameCount);
        else
        {
            ss->outWave.Close();
#ifdef Q_OS_UNIX
            sync();     // make sure it goes to disk
#endif
        }

        mutex.lock();
        ++writeIndex;

        bufferNotFull.wakeAll();
        mutex.unlock();
    }
}

/*static*/
//==============================================================================
RtAudioSoundSystem *RtAudioSoundSystem::createSoundSystem()
{
   return new RtAudioSoundSystem();
}
//==============================================================================
int audioCallback( void *outputBuffer, void *inputBuffer,
                                unsigned int nFrames,
                                double streamTime,
                                RtAudioStreamStatus status,
                                void *userData )
{
    RtAudioSoundSystem *qss = static_cast<RtAudioSoundSystem *>(userData);
    return qss->audioCallback(outputBuffer, inputBuffer, nFrames, streamTime, status);
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
  , wThread(0)
{
    try
    {
       audio = new RtAudio();

       wThread = new RiffWriter(this);
       wThread->start();

       unsigned int devices = audio->getDeviceCount();
       RtAudio::DeviceInfo info;
       for ( unsigned int i=0; i<devices; i++ ) {
         info = audio->getDeviceInfo( i );
         if ( info.probed == true ) {
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

   wThread->terminated = true;
   bufferNotEmpty.wakeAll();
   wThread->wait();
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
bool RtAudioSoundSystem::initialise( QString &/*errmess*/ )
{

    compressor.setSampleRate(sampleRate);
    compressor.setWindow(10);       // milliseconds
    compressor.setThresh( -10 );
    compressor.setRatio( 0.1 );
    compressor.setAttack( 1.0 );     // 1ms seems like a good look-ahead to me
    compressor.setRelease( 10.0 ); // 10ms release is good
    compressor.initRuntime();
    try
    {
        RtAudio::StreamParameters outParams;
        RtAudio::StreamParameters inParams;
        RtAudio::StreamOptions soptions;

        unsigned int bufferFrames = FRAMESAMPLES;

        outParams.deviceId = 0;
        outParams.firstChannel = 0;
        outParams.nChannels = 2;

        inParams.deviceId = 0;
        inParams.firstChannel = 0;
        inParams.nChannels = 2;

        soptions.flags = 0;
        soptions.numberOfBuffers = FRAMES;
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
    }

    return true;
}
int RtAudioSoundSystem::setRate(int rate)
{
   sampleRate = rate;
   return sampleRate;
}

void RtAudioSoundSystem::setVolumeMults(qreal record, qreal replay, qreal passThrough)
{
    // input levels are dB, so the actual multiplier is 10**(level/10)
    // BUT level is already * 10, so we need /100
    recordMult = qPow(10, record/100);
    replayMult = qPow(10, replay/100);
    passThroughMult = qPow(10, passThrough/100);
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
        memset(outputBuffer, 0, nFrames * 2 * 2);   // 2 bytes, 2 channels
    }

    if (inputBuffer && nFrames)
    {
        // ALWAYS apply compressor to input, so it continues to adapt
        int16_t * q = reinterpret_cast<  int16_t * > ( inputBuffer );
        int16_t * m = reinterpret_cast< int16_t * > ( outputBuffer );
        int16_t maxvol = 0;
        qreal sqaccum = 0.0;
        qreal volmult = (inputEnabled?recordMult:passThroughMult);

        for (unsigned int i = 0; i < nFrames ; i++)
        {
            double initi1 = q[i * 2];
            double initi2 = q[i * 2 + 1];

            double s1 = initi1;
            double s2 = initi2;

            s1 /= 32768.0;
            s2 /= 32768.0;

            compressor.process(s1, s2);

            s1 *= 32768.0;
            s2 *= 32768.0;

            qreal val1 = s1 * volmult;
            qreal val2 = s2 * volmult;

            if (val1 > 32767.0)
                val1 = 32767.0;
            if (val1 < -32767.0)
                val1 = -32767.0;

            if (val2 > 32767.0)
                val2 = 32767.0;
            if (val2 < -32767.0)
                val2 = -32767.0;

            if (passThroughEnabled)
            {
                m[i * 2] = static_cast<qint16>(val1);
                m[i * 2 + 1] = static_cast<qint16>(val2);
            }
            if (inputEnabled)
            {
                q[i * 2] = static_cast<qint16>(val1);
                q[i * 2 + 1] = static_cast<qint16>(val2);
            }

            int16_t sample = abs( (val1 + val2)/2 );
            if ( sample > maxvol )
               maxvol = sample;

            sqaccum += sample * sample;
        }
        if (inputEnabled || passThroughEnabled)
        {
            qreal rmsval = sqrt(sqaccum/nFrames);
            SoundSystemDriver::getSbDriver() ->WinVUCallback( maxvol, rmsval, nFrames );
        }

        if (inputEnabled)
        {
            mutex.lock();
            if (recIndex - writeIndex >= RINGBUFFERSIZE - 1)
                bufferNotFull.wait(&mutex);
            mutex.unlock();

            inBuffs[recIndex % RINGBUFFERSIZE].frameCount = nFrames;
            memcpy(inBuffs[recIndex % RINGBUFFERSIZE].buff, inputBuffer, nFrames * 4);

            mutex.lock();
            ++recIndex;
            bufferNotEmpty.wakeAll();
            mutex.unlock();
//            writeDataToFile(inputBuffer, nFrames);
        }
    }
    if (outputBuffer != NULL && nFrames > 0 && outputEnabled )
    {
        int16_t maxvol = 0;
        qreal rmsval = 0.0;
        readFromFile(outputBuffer, nFrames, maxvol, rmsval);

        SoundSystemDriver::getSbDriver() ->WinVUCallback( maxvol, rmsval, nFrames );
    }

    /*
   To continue normal stream operation, the RtAudioCallback function
   should return a value of zero.  To stop the stream and drain the
   output buffer, the function should return a value of one.  To abort
   the stream immediately, the client should return a value of two.      */
    return 0;
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
     SoundSystemDriver::getSbDriver() ->WinVUCallback( 0, 0, 0 );
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
     mutex.lock();
     if (recIndex - writeIndex >= RINGBUFFERSIZE - 1)     // not correct... we want "caught up"
         bufferNotFull.wait(&mutex);
     mutex.unlock();

     inBuffs[recIndex%RINGBUFFERSIZE].frameCount = 0;  // mark to close

     mutex.lock();
     ++recIndex;

     bufferNotEmpty.wakeAll();
     mutex.unlock();
}
bool RtAudioSoundSystem::startInput( QString fn )
{
    // open fname, assign a text(?)
    // startInput() will also be called later

    // Should we do this in the writer thread?
    recIndex = 0;
    writeIndex = 0;

    if ( outWave.OpenForWrite( fn.toLatin1(), sampleRate, 16, 2 ) == DDC_SUCCESS )
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
        DDCRET ret = outWave.WriteData ( q, nFrames * 2 );   // size is numdata
        if ( ret != DDC_SUCCESS )
        {
            return;
        }
    }
}
void RtAudioSoundSystem::readFromFile(void *outputBuffer, unsigned int nFrames, int16_t &maxvol, qreal &rmsval)
{
    if (outputBuffer && nFrames)
    {
        int16_t *q = reinterpret_cast< int16_t * > ( outputBuffer );

        qint64 len = nFrames * 2 * 2;

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
                q += total/2;
                pipDelayBytes -= total;
            }
            else
            {
                if (!p_buffer.isEmpty())
                {
                    total = qMin((p_buffer.size() - p_pos), len);
                    memcpy(q, p_buffer.constData() + p_pos, total);
                    q += total/2;
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

                const int16_t *m = reinterpret_cast< const int16_t * > ( &m_buffer.constData()[m_pos] );

                qreal mult = replayMult;
                if (tone)
                    mult = 1.0;

                for (int i = 0; i < total/2; i++)
                {
                    qreal val =*m++ * mult;
                    if (val > 32767.0)
                        val = 32767.0;
                    if (val < -32767.0)
                        val = -32767.0;
                    *q++ = static_cast<qint16>(val);
                }
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

        // should already have any gain multiplication done
         q = reinterpret_cast< int16_t * > ( outputBuffer );

         // determine max for VU meter
        qreal sqaccum = 0.0;
        for ( unsigned int i = 0; i < nFrames; i++ )
        {
           int16_t sample = abs( *q++ );
           if ( sample > maxvol )
              maxvol = sample;

           sqaccum += sample * sample;
        }

        rmsval = sqrt(sqaccum/nFrames);
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

        tone = fname.isEmpty();

        setData(dataptr, samples);
        KeyerAction * sba = KeyerAction::getCurrentAction();
        if (sba && sba->tailWithPip)
        {
            long psamples = SoundSystemDriver::getSbDriver() ->pipSamples * 2;
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
    SoundSystemDriver::getSbDriver() ->WinVUCallback( 0, 0, 0 );
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
    SoundSystemDriver::getSbDriver() ->WinVUCallback( 0, 0, 0 );
    return true;
}
