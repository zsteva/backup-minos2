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

#include "base_pch.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wold-style-cast"
// as we don't want to change rtaudio.h...
#include "rtaudio.h"
#pragma GCC diagnostic pop

//#include <QAudioInput>
//#include <QAudioOutput>
//#include <QIODevice>

#include "riff.h"

class RtAudio;
class RtAudioSoundSystem: public QObject
{
    Q_OBJECT

private slots:

protected:
    void writeDataToFile(void *inp, int nFrames);
    void readFromFile(void *outputBuffer, unsigned int nFrames);

public:
    RtAudioSoundSystem();
    virtual ~RtAudioSoundSystem();

    virtual bool initialise( QString &errmess );

    virtual int setRate(int rate);

    virtual bool startDMA( bool play, const QString &fname );
    virtual void stopDMA();

    static RtAudioSoundSystem *createSoundSystem();

    void startOutput();
    void stopOutput();
    void startInput();
    void stopInput();
    bool startInput( QString fn );

    virtual bool startMicPassThrough();
    virtual bool stopMicPassThrough();

    void setData(int16_t *data, int len);
    void setPipData(int16_t *data, int len, int delayLen);

    int16_t *dataptr;
    long samples;

    int audioCallback( void *outputBuffer, void *inputBuffer,
                                    unsigned int nFrames,
                                    double streamTime,
                                    RtAudioStreamStatus status );

private:

    RtAudio *audio;

    // internal values
    unsigned int sampleRate;

    bool playingFile;
    bool recordingFile;
    bool passThrough;

    bool inputEnabled;
    bool outputEnabled;
    bool passThroughEnabled;

    qint64 m_pos;
    qint64 p_pos;
    QByteArray m_buffer;
    QByteArray p_buffer;
    long pipDelayBytes;

    WaveFile outWave;

};

#endif
