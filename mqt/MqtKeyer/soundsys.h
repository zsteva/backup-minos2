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
#include "RtAudio.h"
#pragma GCC diagnostic pop

#include "riff.h"
#include "SimpleComp.h"

class RtAudio;
class RtAudioSoundSystem;

class RiffWriter : public QThread
{
    Q_OBJECT

     RtAudioSoundSystem *ss;
public:
     bool terminated;
    RiffWriter(RtAudioSoundSystem *parent = NULL) ;
    virtual ~RiffWriter();

    virtual void run() Q_DECL_OVERRIDE;

};
class LPFilter
{
    int mNumChannels;
    // past data
    double mZx[4];
    double mZy[4];
    // filter coefficients
    double a0;
    double a1;
    double a2;
    double b1;
    double b2;
public:
    LPFilter(){}
    void initialise(int channels, double corner, double sampleRate);
    inline double filterSample (const double inSample, const int channel);
};
class RtAudioSoundSystem: public QObject
{
    Q_OBJECT

    chunkware_simple::SimpleCompRms compressor;
    LPFilter lpFilter;

private slots:

protected:
    void readFromFile(void *outputBuffer, unsigned int nFrames, int16_t &maxvol, qreal &rmsval);

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

    void setVolumeMults(qreal record, qreal replay, qreal passThrough);

    void setData(int16_t *data, int len);
    void setPipData(int16_t *data, int len, int delayLen);

    int16_t *dataptr;
    long samples;

    WaveFile outWave;
    void writeDataToFile(void *inp, int nFrames);
    RiffWriter *wThread;

    int audioCallback( void *outputBuffer, void *inputBuffer,
                                    unsigned int nFrames,
                                    double streamTime,
                                    RtAudioStreamStatus status );

private:

    RtAudio *audio;

    // internal values
    unsigned int sampleRate;

    bool tone;

    bool playingFile;
    bool recordingFile;
    bool passThrough;

    bool inputEnabled;
    bool outputEnabled;
    bool passThroughEnabled;

    qreal recordMult;
    qreal replayMult;
    qreal passThroughMult;

    qint64 m_pos;
    qint64 p_pos;
    QByteArray m_buffer;
    QByteArray p_buffer;
    long pipDelayBytes;

};

#endif
