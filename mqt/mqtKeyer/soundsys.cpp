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

#include <process.h>
#include "keyctrl.h"
#include "keyconf.h"
#include "vkmixer.h"
#include "sbdriver.h"
#include "soundsys.h"
#include "keyers.h"
#include "writerthread.h"
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
//==============================================================================
MinosAudioIODevice::MinosAudioIODevice(QObject *parent)
    :   QIODevice(parent)
    ,   m_pos(0), p_pos(0)
{
}

MinosAudioIODevice::~MinosAudioIODevice()
{

}

void MinosAudioIODevice::startOutput()
{
    open(QIODevice::ReadOnly);
}

void MinosAudioIODevice::stopOutput()
{
    m_pos = 0;
    p_pos = 0;
    close();
}
void MinosAudioIODevice::startInput()
{
    open(QIODevice::WriteOnly);
}

void MinosAudioIODevice::stopInput()
{
    outWave.Close();
    close();
}
bool MinosAudioIODevice::startInput( QString fn )
{
    // open fname, assign a text(?)
    if ( outWave.OpenForWrite( fn.toLatin1(), 22050, 16, 1 ) == DDC_SUCCESS )
       return true;

    return false;
}

void MinosAudioIODevice::setData(int16_t *data, int len)
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
void MinosAudioIODevice::setPipData(int16_t *data, int len, int delayLen)
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
qint64 MinosAudioIODevice::readData(char *data, qint64 len)
{
    // we have to add in the pip here as well...
    qint64 total = 0;
    if (m_pos >= m_buffer.size())
    {
        // add in pip delay and pip
        if (p_pos >= p_buffer.size())
        {
            trace("Audio readData " + QString::number(len) + " returning " + QString::number(0));
            return 0;
        }
        if (pipDelayBytes > 0)
        {
            qint64 ps = pipDelayBytes;
            total = qMin(ps, len);
            memset(data, 0, total);
            pipDelayBytes -= total;
            trace("pipdelay");
        }
        else
        {
            if (!p_buffer.isEmpty())
            {
                total = qMin((p_buffer.size() - p_pos), len);
                memcpy(data, p_buffer.constData() + p_pos, total);
                p_pos += total;
                trace("pip");
            }
            else
            {
                trace("p_buffer empty");
            }
        }
    }
    else
    {
        if (!m_buffer.isEmpty())
        {
            total = qMin((m_buffer.size() - m_pos), len);
            memcpy(data, m_buffer.constData() + m_pos, total);
            m_pos += total;
            trace("data");
        }
        else
        {
            trace("m_buffer empty");
        }
    }
    KeyerAction * sba = KeyerAction::getCurrentAction();
    if ( sba )
       sba->interruptOK();	// so as we do not time it out immediately
    trace("Audio readData " + QString::number(len) + " returning " + QString::number(total));
    return total;
}

qint64 MinosAudioIODevice::writeData(const char *data, qint64 len)
{
    // data arrives here; we need to write it to the (already open) file,
    // or ditch it

    // OR pass it on to the output device for passthrough?

    int16_t *q = ( int16_t * ) ( data );
    DDCRET ret = outWave.WriteData ( q, len / 2 );   // size is samples
    if ( ret != DDC_SUCCESS )
    {
        return 0;
    }

    return len;
}

qint64 MinosAudioIODevice::bytesAvailable() const
{
    return m_buffer.size() + QIODevice::bytesAvailable();
}
//==============================================================================

//==============================================================================
QtSoundSystem::QtSoundSystem() :
    sampleRate( 0 )
  , qAudioIn(0), qAudioOut(0)
  , maIOdev(0)
{
    maIOdev = new MinosAudioIODevice(this);
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
   return true;
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
                   trace( "All buffers now returned" );
                }
                sba->queueFinished();
             }
        }
        break;

        case QAudio::StoppedState:
        trace("Audio output stopped state");
            // Stopped for other reasons
            if (qAudioOut->error() != QAudio::NoError) {
                // Error handling
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
            if (maIOdev)
                maIOdev->stopInput();
            KeyerAction * sba = KeyerAction::getCurrentAction();
             if ( sba )
             {
                if ( sblog )
                {
                   trace( "All buffers now returned" );
                }
                sba->queueFinished();
             }
        }
        break;

        case QAudio::StoppedState:
            trace("Audio input stopped state");
            // Stopped for other reasons
            if (qAudioOut->error() != QAudio::NoError) {
                // Error handling
            }
            else
            {
                KeyerAction * sba = KeyerAction::getCurrentAction();
                 if ( sba )
                 {
                    if ( sblog )
                    {
                       trace( "All buffers now returned" );
                    }
                    sba->queueFinished();
                 }
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

        maIOdev->setData(dataptr, samples);                           // with DO_FILE_PIP this works
        KeyerAction * sba = KeyerAction::getCurrentAction();
        if (sba && sba->tailWithPip)
        {
            long psamples = SoundSystemDriver::getSbDriver() ->pipSamples;  // but this is inverted!
            int16_t *pdataptr = SoundSystemDriver::getSbDriver() ->pipptr;
            maIOdev->setPipData(pdataptr, psamples, sba->pipStartDelaySamples);
        }
        maIOdev->startOutput();
        qAudioOut->start(maIOdev);
   }
   else
   {
       if ( sblog )
       {
          trace( "(StartDMA) Starting input" );
       }

         if ( !maIOdev->startInput( fname ) )
            return false;

         maIOdev->startInput();
         qAudioIn->start(maIOdev);
   }
   return true;
}
void QtSoundSystem::stopDMA()
{
//    Here we need to stop input/output
    if (qAudioIn)
        qAudioIn->stop();
    trace( "QtSoundSystem::stopDMA" );
}
