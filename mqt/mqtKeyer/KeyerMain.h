#ifndef KEYERMAIN_H
#define KEYERMAIN_H

#include "base_pch.h"
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>
#include <QComboBox>
#include <QMainWindow>

#include "AlsaVolume.h"

namespace Ui {
class KeyerMain;
}

class KeyerMain : public QMainWindow
{
    Q_OBJECT
    void closeEvent(QCloseEvent * event);
    void resizeEvent(QResizeEvent *event);
    void setMixerCombo(QComboBox *combo, QList<QAudioDeviceInfo> audioDevices, QAudioFormat *qaf);
    void setMixerCombo(QComboBox *combo, QVector<Device> devices);
    void setMixerCombo(QComboBox *combo, PxDev &devices);

    void saveMixerSetting(QSettings &keyerSettings, QString key, QComboBox *combo);
    void applyMixerSetting(QSettings &keyerSettings, QString key, QComboBox *combo);

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

    void on_cardCombo_currentIndexChanged(int index);

    void on_inputDeviceCombo_currentIndexChanged(int index);

    void on_outputDeviceCombo_currentIndexChanged(int index);

    void on_inputControlCombo_currentIndexChanged(int index);

    void on_outputControlCombo_currentIndexChanged(int index);

    void on_passthruControlCombo_currentIndexChanged(int index);

private:
    Ui::KeyerMain *ui;
    void syncSetLines();
    QTimer LineTimer;
    QTimer CaptionTimer;

    QVector<Card> cards;

    px_mixer Px;

    int currCardIndex;
    int currInputIndex;
    int currOutputIndex;

    bool PTT;
    bool keyline;
    bool PTTRef;
    bool L1Ref;
    bool L2Ref;

    bool recordWait;
    bool recording;

    bool inVolChange;
    bool inInit;

};
#endif // KEYERMAIN_H
