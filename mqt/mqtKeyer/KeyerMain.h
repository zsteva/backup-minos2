#ifndef KEYERMAIN_H
#define KEYERMAIN_H

#include "base_pch.h"
#include <QComboBox>
#include <QMainWindow>
#include <QProcess>

#include "VKMixer.h"

namespace Ui {
class KeyerMain;
}

class KeyerMain : public QMainWindow
{
    Q_OBJECT
    Ui::KeyerMain *ui;

public:
    explicit KeyerMain(QWidget *parent = 0);
    ~KeyerMain();

    void setLines( bool PTTOut, bool PTTIn, bool L1, bool L2, bool key );
    void recvolcallback( unsigned int rmsvol , unsigned int peakvol, int samples );
    void outvolcallback(unsigned int rmsvol , unsigned int peakvol, int samples);

private slots:

    void CaptionTimerTimer();

    void LineTimerTimer();


    void on_recordButton_clicked();

    void on_playButton_clicked();

    void on_showButton_clicked();

    void on_stopButton_clicked();

    void on_AutoRepeatCheckBox_clicked();

    void on_PipCheckBox_clicked();

    void on_delayEdit_valueChanged(const QString &arg1);

    void on_tuneButton_clicked();

    void on_twoToneButton_clicked();

    void on_aboutButton_clicked();

    void on_setupBrowseButton_clicked();

    void on_started();
    void on_finished(int, QProcess::ExitStatus exitStatus);
    void on_error(QProcess::ProcessError error);

    void on_readyReadStandardError();
    void on_readyReadStandardOutput();


    void on_saveAlsaButton_clicked();

    void on_restoreAlsaButton_clicked();

private:
    void syncSetLines();
    QTimer LineTimer;
    QTimer CaptionTimer;

    VKMixer *mixer;

    bool PTT;
    bool keyline;
    bool PTTRef;
    bool L1Ref;
    bool L2Ref;

    bool recordWait;
    bool recording;

    bool inVolChange;

    QProcess *runner;


    void runAlsaScript(const QString &alsaFileName, const QString &command);

    virtual void closeEvent(QCloseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void changeEvent( QEvent* e ) override;
};
#endif // KEYERMAIN_H
