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


class MinosAudioOut : public QIODevice
{
    Q_OBJECT

protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;

public:
    MinosAudioOut(QObject *parent);
    ~MinosAudioOut();

    void start();
    void stop();

    virtual qint64 bytesAvailable() const override;

    void setData(int16_t *data, int len);
    void setPipData(int16_t *data, int len, int delayLen);

private:
    qint64 m_pos;
    qint64 p_pos;
    QByteArray m_buffer;
    QByteArray p_buffer;
    long pipDelayBytes;
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
    QAudioOutput *qout;
    QAudioInput *qin;

    MinosAudioOut *maout;

      // internal values
      WriterThread *wt;
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
      void handleStateChanged(QAudio::State newState);

};

#endif
