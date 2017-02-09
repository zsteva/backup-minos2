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

#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>
#include "riff.h"


class MinosAudioIODevice : public QIODevice
{
    Q_OBJECT

protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;

public:
    MinosAudioIODevice(QObject *parent);
    ~MinosAudioIODevice();

    void startOutput();
    void stopOutput();
    void startInput();
    void stopInput();
    bool startInput( QString fn );

    virtual qint64 bytesAvailable() const override;

    void setData(int16_t *data, int len);
    void setPipData(int16_t *data, int len, int delayLen);

private:
    qint64 m_pos;
    qint64 p_pos;
    QByteArray m_buffer;
    QByteArray p_buffer;
    long pipDelayBytes;

    WaveFile outWave;
};


class WriterThread;
class QtSoundSystem: public QObject
{
    Q_OBJECT
protected:
   int cfgrate;

public:
   volatile int done;
   volatile long samplesremaining;
   volatile long now;
   volatile int sbactive;

   int16_t *dataptr;
   long samples;

   int16_t *pipdataptr;
   long pipSamples;

   int mset;

   static QtSoundSystem *createSoundSystem();

private:
    bool startInput( QString fname );

    QAudioOutput *qAudioOut;
    QAudioInput *qAudioIn;

    MinosAudioIODevice *maIOdev;

      // internal values
    int sampleRate;
public:

    QtSoundSystem();
    virtual ~QtSoundSystem();

    virtual bool initialise( QString &errmess );

    virtual void terminate();
    virtual int setRate();

    virtual bool startDMA( bool play, const QString &fname );
    virtual void stopDMA();

private slots:
      void handleOutStateChanged(QAudio::State newState);
      void handleInStateChanged(QAudio::State newState);

};

#endif
