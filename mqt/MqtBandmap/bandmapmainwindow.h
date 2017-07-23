#ifndef BANDMAPMAINWINDOW_H
#define BANDMAPMAINWINDOW_H

#include <QMainWindow>
#include "mqtUtils_pch.h"

class QLabel;
class QComboBox;
class SetupDialog;
class QGraphicsScene;
class RigControl;
class Bandmap;
class FreqDial;
class TextMarker;


namespace Ui {
class BandMapMainWindow;
}

class BandMapMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BandMapMainWindow(QWidget *parent = 0);
    ~BandMapMainWindow();

private:
    Ui::BandMapMainWindow *ui;
    StdInReader stdinReader;
    QGraphicsScene *bandmapScene;
    FreqDial *dial;
    QComboBox *selectRadio;
    SetupDialog *selectRig;
    RigControl  *radio;
    Bandmap *bandmap;
    QLabel *status;
    QTimer *timer;
    TextMarker *callSignText;
    int pollTime;
    QString sfreq;
    double oldFreq = 0;

    void initActionsConnections();
    void initSelectRadioBox();
    void openRadio();
    void closeRadio();
    QString convertStringFreq(double);

    void setPolltime(int);
    int getPolltime();
    void showStatusMessage(const QString &);
    void hamlibError(int errorCode);



private slots:

    void onStdInRead(QString);

    void upDateRadio();
    void getFrequency();
    void displayFreq(double);
    void drawDial(double frequency);

};

#endif // BANDMAPMAINWINDOW_H
