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

public:
    MinosAudioOut(QObject *parent);
    ~MinosAudioOut();

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable() const;

    void setData(qint16 *data, int len);

private:
    qint64 m_pos;
    QByteArray m_buffer;
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

   qint16 *dataptr;
   long samples;

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
