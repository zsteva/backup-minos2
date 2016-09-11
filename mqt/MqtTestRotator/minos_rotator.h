#ifndef MINOS_ROTATOR_H
#define MINOS_ROTATOR_H

#include <QMainWindow>
#include <QObject>
#include <QtSerialPort/QSerialPort>

class QLabel;
class SettingsDialog;
class Yaesu;

namespace Ui {
    class Minos_rotator;
}


class Minos_rotator : public QMainWindow
{
    Q_OBJECT

public:
    explicit Minos_rotator(QWidget *parent = 0);
    ~Minos_rotator();

    // serial port to rotator controller
    void openSerialPort();
    void closeSerialPort();
    void readData();
    void handleError(QSerialPort::SerialPortError error);

    // setup connections
    void initActionsConnections();

public slots:
    void writeData(const char* data);

private:
    void showStatusMessage(const QString &);

    Ui::Minos_rotator *ui;
    Yaesu *rotator;
    QLabel *status;
    SettingsDialog *settings;
    QSerialPort *serial;

};

#endif // MINOS_ROTATOR_H
