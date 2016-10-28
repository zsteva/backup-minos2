#ifndef ROTATORMAINWINDOW_H
#define ROTATORMAINWINDOW_H

#include "base_pch.h"
#include <QMainWindow>
#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QKeyEvent>
#include <QPushButton>



#define NUM_PRESETS 10
#define NUM_ROTATOR_PROTOCOLS 2

class QLabel;
class QComboBox;
class SetupDialog;
class MinosCompass;
//class Yaesu;
class RotControl;
class EditPresetsDialog;


namespace Ui {
class RotatorMainWindow;
}

class RotatorLogic;

class RotatorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RotatorMainWindow(QWidget *parent = 0);
    ~RotatorMainWindow();


    void setPolltime(int interval);
    int getPolltime();

    // setup connections
    void initActionsConnections();

signals:
    void escapePressed();
    void sendBackBearing(QString);
    void presetRotateTo();

private:
    Ui::RotatorMainWindow *ui;


    RotatorLogic *rl;

    QTimer LogTimer;

    QComboBox *selectAntenna;
    QPushButton* presetButtons[NUM_PRESETS];
    MinosCompass* compassDial;
    RotControl  *rotator;
    QLabel *status;
    SetupDialog *selectRotator;
    EditPresetsDialog *editPresets;
    QTimer *timer;
    int pollTime;
    QString backBearingmsg;
    QString presetName[NUM_PRESETS];
    QString presetBearing[NUM_PRESETS];

    void openRotator();
    void closeRotator();

    void refreshPresetLabels();
    void showStatusMessage(const QString &);
    void readPresets();

    void initSelectAntennaBox();

    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *);

    void logMessage( QString s );

public slots:
//    void writeData(QByteArray);
//    void openSerialPort();
    void rotateToController();
    void displayBackBearing(const QString);
    void updatePresetLabels();
    void clickedPreset(int buttonNumber);

protected slots:
    void upDateAntenna();
    void request_bearing();
    void rotateCW();
    void rotateCCW();
    void stopRotation();

private slots:
    void LogTimerTimer( );
    void onSetRotation(int direction, int angle);
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();


};

#endif // ROTATORMAINWINDOW_H
