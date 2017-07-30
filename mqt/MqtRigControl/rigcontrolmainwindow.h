#ifndef RIGCONTROLMAINWINDOW_H
#define RIGCONTROLMAINWINDOW_H

#include <QMainWindow>
#include "mqtUtils_pch.h"
#include "rigcontrol.h"

class QLabel;
class QComboBox;
class SetupDialog;
class RigControl;

#define NUM_VFO 2


namespace Ui {
class RigControlMainWindow;
}

class RigControlMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RigControlMainWindow(QWidget *parent = 0);
    ~RigControlMainWindow();

private:
    Ui::RigControlMainWindow *ui;
    StdInReader stdinReader;
    QComboBox *selectRadio;
    SetupDialog *selectRig;
    RigControl  *radio;
    QLabel *status;
    QTimer *timer;
    int pollTime;
    QString sfreq;
//    double oldFreq = 0;
    double curFreq = 0;
    vfo_t curVFO;
    freq_t frequency;            // frequency
    rmode_t rmode;          // radio mode of operation
    pbwidth_t width;
    vfo_t vfo;              // vfo selection
    int strength;           // S-Meter level
    int retcode;            // generic return code from functions

    double curVfoFrq[NUM_VFO];
    rmode_t curMode[NUM_VFO];
    pbwidth_t curRxWidth[NUM_VFO];



    void initActionsConnections();
    void initSelectRadioBox();
    void openRadio();
    void closeRadio();
    QString convertStringFreq(double);

    void setPolltime(int);
    int getPolltime();
    void showStatusMessage(const QString &);
    void hamlibError(int errorCode);

protected:
//    void resizeEvent(QResizeEvent *event) override;
//    void paintEvent(QPaintEvent *event) override;

private slots:

    void onStdInRead(QString);

    void upDateRadio();
    void getFrequency();
    void displayFreq(double);
//    void drawDial(double frequency);

private slots:
    void updateFreq(double frequency);

signals:
    void frequency_updated(double frequency);


};

#endif // RIGCONTROLMAINWINDOW_H
