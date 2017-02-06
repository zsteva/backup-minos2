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
MinosAudioOut::MinosAudioOut(QObject *parent)
    :   QIODevice(parent)
    ,   m_pos(0), p_pos(0)
{
}

MinosAudioOut::~MinosAudioOut()
{

}

void MinosAudioOut::start()
{
    open(QIODevice::ReadOnly);
}

void MinosAudioOut::stop()
{
    m_pos = 0;
    p_pos = 0;
    close();
}
void MinosAudioOut::setData(int16_t *data, int len)
{
    m_buffer.resize(len * sizeof(uint16_t));
    unsigned char *ptr = reinterpret_cast<unsigned char *>(m_buffer.data());

    for (int i = 0; i < len; i++)
    {
        uint16_t value = data[i];
        qToLittleEndian<uint16_t>(value, ptr);
        ptr += 2;
    }
    m_pos = 0;
}
void MinosAudioOut::setPipData(int16_t *data, int len, int delayLen)
{
    pipDelayBytes = delayLen * sizeof(uint16_t);
    p_buffer.resize(len * sizeof(uint16_t));
    unsigned char *ptr = reinterpret_cast<unsigned char *>(p_buffer.data());

    for (int i = 0; i < len; i++)
    {
        uint16_t value = data[i];
        qToLittleEndian<uint16_t>(value, ptr);
        ptr += 2;
    }
    p_pos = 0;
}
qint64 MinosAudioOut::readData(char *data, qint64 len)
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

qint64 MinosAudioOut::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 MinosAudioOut::bytesAvailable() const
{
    return m_buffer.size() + QIODevice::bytesAvailable();
}

//==============================================================================
QtSoundSystem::QtSoundSystem() :
    sampleRate( 0 ), wt(0)
  , qin(0), qout(0)
  , maout(0)
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

bool QtSoundSystem::initialise( QString &/*errmess*/ )
{
    QAudioFormat qaf;
    qaf.setChannelCount(1);
    qaf.setSampleRate(sampleRate);
    qaf.setSampleSize(16);
    qaf.setSampleType(QAudioFormat::SignedInt);
    qaf.setByteOrder(QAudioFormat::LittleEndian);
    qaf.setCodec("audio/pcm");

    qin = new QAudioInput(qaf);

    qout = new QAudioOutput(qaf);
    connect(qout, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
   return true;
}
void QtSoundSystem::handleStateChanged(QAudio::State newState)
{
//    enum Error { NoError, OpenError, IOError, UnderrunError, FatalError };
//    enum State { ActiveState, SuspendedState, StoppedState, IdleState };

    switch (newState) {
        case QAudio::IdleState:
        {
        trace("Audio idle state");
            // Finished playing (no more data)
//            qout->stop();
//            maout->close();
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
        trace("Audio stopped state");
            // Stopped for other reasons
            if (qout->error() != QAudio::NoError) {
                // Error handling
            }
            break;

        default:
            // ... other cases as appropriate
        /*
    enum Error { NoError, OpenError, IOError, UnderrunError, FatalError };
    enum State { ActiveState, SuspendedState, StoppedState, IdleState };
    enum Mode { AudioInput, AudioOutput };
        */
        trace("Audio other state " + QString::number(newState));
            break;
    }
}
void QtSoundSystem::terminate()
{

}
bool QtSoundSystem::startDMA( bool play, const QString &/*fname*/ )
{
   // start input / output

   if ( play )
   {
      if ( sblog )
      {
         trace( "Starting output" );
      }
        if (maout == 0)
            maout = new MinosAudioOut(qout);

        maout->setData(dataptr, samples);                           // with DO_FILE_PIP this works
        KeyerAction * sba = KeyerAction::getCurrentAction();
        if (sba && sba->tailWithPip)
        {
            long psamples = SoundSystemDriver::getSbDriver() ->pipSamples;  // but this is inverted!
            int16_t *pdataptr = SoundSystemDriver::getSbDriver() ->pipptr;
            maout->setPipData(pdataptr, psamples, sba->pipStartDelaySamples);
        }
        maout->start();
        qout->start(maout);
   }
   else
   {

     return false;
   }
   return true;
}
void QtSoundSystem::stopDMA()
{
   trace( "QtSoundSystem::stopDMA" );
}
