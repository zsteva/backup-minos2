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

//#include <process.h>
#include "keyctrl.h"
#include "keyconf.h"
#include "VKMixer.h"
#include "sbdriver.h"
#include "soundsys.h"
#include "keyers.h"
#include "WriterThread.h"
#include "keyerlog.h"
#include "riff.h"

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
qreal QtSoundSystem::getKeyerPlaybackVolume()
{
    return qAudioOut->volume();
}

qreal QtSoundSystem::getKeyerRecordVolume()
{
    return qAudioIn->volume();
}

void QtSoundSystem::setKeyerPlaybackVolume(qreal vol)
{
    qAudioOut->setVolume(vol);
}

void QtSoundSystem::setKeyerRecordVolume(qreal vol)
{
    qAudioIn->setVolume(vol);
}

void QtSoundSystem::startOutput()
{
    outDev = qAudioOut->start();    // restarts as necessary
}

void QtSoundSystem::stopOutput()
{
    outDev = qAudioOut->start();    // restarts as necessary

}
void QtSoundSystem::startInput()
{
}

void QtSoundSystem::stopInput()
{
    outWave.Close();
}
bool QtSoundSystem::startInput( QString fn )
{
    // open fname, assign a text(?)
    if ( outWave.OpenForWrite( fn.toLatin1(), 22050, 16, 1 ) == DDC_SUCCESS )
       return true;

    return false;
}

void QtSoundSystem::setData(int16_t *data, int len)
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
void QtSoundSystem::setPipData(int16_t *data, int len, int delayLen)
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

void QtSoundSystem::writeDataToFile(QByteArray &inp)
{
    // data arrives here; we need to write it to the (already open) file,

    int len = inp.length();
    if (len)
    {
        const int16_t *q = reinterpret_cast< const int16_t * > ( inp.data() );
        DDCRET ret = outWave.WriteData ( q, len / 2 );   // size is samples
        if ( ret != DDC_SUCCESS )
        {
            return;
        }
    }
}
void QtSoundSystem::readFromFile()
{
    qint64 len = qAudioOut->bytesFree();

    if (len)
    {
        QByteArray data;

        // we have to add in the pip here as well...
        qint64 total = 0;
        if (m_pos >= m_buffer.size())
        {
            // add in pip delay and pip
            if (p_pos >= p_buffer.size())
            {
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
                trace("m_buffer empty");
            }
        }
        KeyerAction * sba = KeyerAction::getCurrentAction();
        if ( sba )
           sba->interruptOK();	// so as we do not time it out immediately

        const int16_t * q = reinterpret_cast< const int16_t * > ( data.constData() );
         int16_t maxvol = 0;

         // determine max for VU meter
        for ( int i = 0; i < total / 2; i++ )
        {
           int16_t sample = abs( *q++ );
           if ( sample > maxvol )
              maxvol = sample;
        }

         SoundSystemDriver::getSbDriver() ->WinVUOutCallback( maxvol * shortmult );

         outDev->write(data);
    }
}

void QtSoundSystem::passThroughData(QByteArray &inp)
{
    if (inp.size() && currentKeyer)
    {
        bool ptt = currentKeyer->pttState;
        int flen = qAudioOut->bytesFree();
        int ilen = inp.size();
        int blen = qAudioOut->bufferSize();
        if (ptt && flen)
        {
            int len = qMin(flen, ilen);
            const int16_t * q = reinterpret_cast< const int16_t * > ( inp.constData() );
             int16_t maxvol = 0;

             // determine max for VU meter
            for ( int i = 0; i < len / 2; i++ )
            {
               int16_t sample = abs( *q++ );
               if ( sample > maxvol )
                  maxvol = sample;
            }

             SoundSystemDriver::getSbDriver() ->WinVUOutCallback( maxvol * shortmult );
            outDev->write(inp);
        }
        else
        {
            SoundSystemDriver::getSbDriver() ->WinVUOutCallback( 0 );
        }
    }
}

//==============================================================================

//==============================================================================
QtSoundSystem::QtSoundSystem() :
    sampleRate( 0 )
  , qAudioIn(0), qAudioOut(0)
  , playingFile(false)
  , recordingFile(false)
  , passThrough(false)
  , m_pos(0), p_pos(0)
{

}
QtSoundSystem::~QtSoundSystem()
{
   stopDMA();
}
int QtSoundSystem::setRate()
{
   sampleRate = 22050;
   return 22050;
}

bool QtSoundSystem::initialise( QString &/*errmess*/ )
{
    QAudioFormat qaf;
    qaf.setChannelCount(1);
    qaf.setSampleRate(sampleRate);
    qaf.setSampleSize(16);
    qaf.setSampleType(QAudioFormat::SignedInt);
    qaf.setByteOrder(QAudioFormat::LittleEndian);
    qaf.setCodec("audio/pcm");

    qAudioIn = new QAudioInput(qaf);
    connect(qAudioIn, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleInStateChanged(QAudio::State)));

    qAudioOut = new QAudioOutput(qaf);
    connect(qAudioOut, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleOutStateChanged(QAudio::State)));

    inDev = qAudioIn->start();
    outDev = qAudioOut->start();

    connect(&pushTimer, SIGNAL(timeout()), this, SLOT(handle_pushTimer_timeout()));
    pushTimer.start(10);
   return true;
}
void QtSoundSystem::handle_pushTimer_timeout()
{
    // read input, buffer

    QByteArray inp = inDev->readAll();

    if (recordingFile)
    {
        // if recording write buffer to RIFF, return losing input
        writeDataToFile(inp);
    }
    else if (playingFile)
    {
        // if playing file, push any remaining data to output losing input
        //          and do pip tail after delay if required
        readFromFile();

    }
    else if (passThrough)
    {
        // if passthrough, push read data to output
        passThroughData(inp);
    }
    else
    {
    // what do we do with any remaining input? Just lose it?
        SoundSystemDriver::getSbDriver() ->WinVUInCallback( 0 );
        SoundSystemDriver::getSbDriver() ->WinVUOutCallback( 0 );
    }
}

void QtSoundSystem::handleOutStateChanged(QAudio::State newState)
{
//    enum Error { NoError, OpenError, IOError, UnderrunError, FatalError };
//    enum State { ActiveState, SuspendedState, StoppedState, IdleState };

    switch (newState) {
        case QAudio::IdleState:
        {
        trace("Audio output idle state");
            // Finished playing (no more data)
            KeyerAction * sba = KeyerAction::getCurrentAction();
             if ( sba )
             {
                if ( sblog )
                {
                   //trace( "All buffers now returned" );
                }
                sba->actionTime = 1;
             }
        }
        break;

        case QAudio::StoppedState:
        {
            trace("Audio output stopped state");
            // Stopped for other reasons
            if (qAudioOut->error() != QAudio::NoError) {
                // Error handling
            }
        }
        break;
        case QAudio::ActiveState:
        {
            // make sure any volume change actually happens
            qAudioOut->setVolume(qAudioOut->volume());
            trace("Audio output active state");
        }
        break;
        default:
            // ... other cases as appropriate
        trace("Audio output other state " + QString::number(newState));
            break;
    }
}
void QtSoundSystem::handleInStateChanged(QAudio::State newState)
{
//    enum Error { NoError, OpenError, IOError, UnderrunError, FatalError };
//    enum State { ActiveState, SuspendedState, StoppedState, IdleState };

    switch (newState) {
        case QAudio::IdleState:
        {
        // this doesn't happen... at least not while recording
            trace("Audio input idle state");
            // Finished playing (no more data)

            stopInput();
            KeyerAction * sba = KeyerAction::getCurrentAction();
             if ( sba )
             {
                if ( sblog )
                {
                   //trace( "All buffers now returned" );
                }
                sba->queueFinished();
             }
        }
        break;

        case QAudio::StoppedState:
            trace("Audio input stopped state");
            // Stopped for other reasons
            if (qAudioIn->error() != QAudio::NoError) {
                // Error handling
            }
            else
            {
                KeyerAction * sba = KeyerAction::getCurrentAction();
                 if ( sba )
                 {
                    if ( sblog )
                    {
                       //trace( "All buffers now returned" );
                    }
                    sba->queueFinished();
                 }
            }
            break;
            case QAudio::ActiveState:
            {
                // make sure any volume change actually happens
                qAudioIn->setVolume(qAudioIn->volume());
                trace("Audio output active state");
            }
            break;

        default:
            // ... other cases as appropriate
            trace("Audio input other state " + QString::number(newState));
            break;
    }
}
void QtSoundSystem::terminate()
{
    if (qAudioIn)
        qAudioIn->stop();
    if (qAudioOut)
        qAudioOut->stop();

    delete qAudioIn;
    delete qAudioOut;
}
bool QtSoundSystem::startDMA( bool play, const QString &fname )
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
void QtSoundSystem::stopDMA()
{
//    Here we need to stop input/output
    trace( "QtSoundSystem::stopDMA" );

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
    SoundSystemDriver::getSbDriver() ->WinVUInCallback( 0 );
    SoundSystemDriver::getSbDriver() ->WinVUOutCallback( 0 );

}
