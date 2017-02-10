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

class WriterThread;
class QtSoundSystem: public QObject
{
    Q_OBJECT

public:
   int16_t *dataptr;
   long samples;

   static QtSoundSystem *createSoundSystem();

private:
    QTimer pushTimer;

    QAudioOutput *qAudioOut;
    QAudioInput *qAudioIn;
    QIODevice *inDev;
    QIODevice *outDev;

      // internal values
    int sampleRate;

    bool playingFile;
    bool recordingFile;
    bool passThrough;

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
      void handle_pushTimer_timeout();

protected:
    void writeDataToFile(QByteArray &inp);
    void readFromFile();
    void passThroughData(QByteArray &inp);

public:
    void startOutput();
    void stopOutput();
    void startInput();
    void stopInput();
    bool startInput( QString fn );

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

#endif
