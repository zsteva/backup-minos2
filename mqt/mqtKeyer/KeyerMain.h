#ifndef KEYERMAIN_H
#define KEYERMAIN_H

#include "base_pch.h"
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>
#include <QComboBox>
#include <QMainWindow>

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
    void recvolcallback( unsigned int vol );
    void outvolcallback( unsigned int vol );

private slots:
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

    void CaptionTimerTimer();

    void LineTimerTimer();

    void on_inputLevelSlider_sliderMoved(int position);

    void on_masterLevelSlider_sliderMoved(int position);

    void on_passthruLevelSlider_sliderMoved(int position);

    void on_outputLevelSlider_sliderMoved(int position);

    void on_cardCombo_currentIndexChanged(int index);

    void on_inputMute_toggled(bool checked);

    void on_masterMute_toggled(bool checked);

    void on_passthruMute_toggled(bool checked);

    void on_outputMute_toggled(bool checked);

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
    bool inInit;

    virtual void closeEvent(QCloseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void changeEvent( QEvent* e ) override;

    void saveMixerSetting(QSettings &keyerSettings, QString key, QComboBox *combo);
    void applyMixerSetting(QSettings &keyerSettings, QString key, QComboBox *combo);
//    void adjustDeviceControls( PxDev *dev, QComboBox *devCombo, QSlider *slider, QCheckBox *muteBox);
};
#endif // KEYERMAIN_H
