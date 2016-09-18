#ifndef KEYERMAIN_H
#define KEYERMAIN_H

#include "base_pch.h"
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>
#include <QMainWindow>

namespace Ui {
class KeyerMain;
}

class KeyerMain : public QMainWindow
{
    Q_OBJECT
    void closeEvent(QCloseEvent * /*event*/);

public:
    explicit KeyerMain(QWidget *parent = 0);
    ~KeyerMain();
    void setLines( bool PTTOut, bool PTTIn, bool L1, bool L2, bool key );

private slots:
    void on_recordButton_clicked();

    void on_playButton_clicked();

    void on_showButton_clicked();

    void on_stopButton_clicked();

    void on_AutoRepeatCheckBox_clicked();

    void on_PipCheckBox_clicked();

    void on_delayEdit_valueChanged(const QString &arg1);

    void on_inputComboBox_currentTextChanged(const QString &arg1);

    void on_outputComboBox_currentTextChanged(const QString &arg1);

    void on_tuneButton_clicked();

    void on_twoToneButton_clicked();

    void on_aboutButton_clicked();

private:
    Ui::KeyerMain *ui;
    void syncSetLines();
    QTimer LineTimer;
    QTimer CaptionTimer;

    bool PTT;
    bool keyline;
    bool PTTRef;
    bool L1Ref;
    bool L2Ref;

    bool recordWait;
    bool recording;

private slots:
    void CaptionTimerTimer();
    void LineTimerTimer();
};
#endif // KEYERMAIN_H
