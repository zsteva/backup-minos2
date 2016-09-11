#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "minoscompass.h"
#include "yaesu.h"
#include "setupdialog.h"
#include "editpresetsdialog.h"
#include <QString>
#include <QLabel>
#include <QMessageBox>
#include <QSignalMapper>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QSettings>
#include <QtDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    presetButtons[0] = ui->presetButton1;
    presetButtons[1] = ui->presetButton2;
    presetButtons[2] = ui->presetButton3;
    presetButtons[3] = ui->presetButton4;
    presetButtons[4] = ui->presetButton5;
    presetButtons[5] = ui->presetButton6;
    presetButtons[6] = ui->presetButton7;
    presetButtons[7] = ui->presetButton8;
    presetButtons[8] = ui->presetButton9;
    presetButtons[9] = ui->presetButton10;

    QSignalMapper *preset_mapper = new QSignalMapper(this);


    for (int i = 0; i < NUM_PRESETS; i++ )
    {
        preset_mapper->setMapping(presetButtons[i], i);
        connect(presetButtons[i], SIGNAL(clicked()), preset_mapper, SLOT(map()));
    }
    connect(preset_mapper, SIGNAL(mapped(int)), this, SLOT(clickedPreset(int)));


    selectRotator = new SetupDialog;
    editPresets = new EditPresetsDialog;
    serial = new QSerialPort(this);
    rotator = new Yaesu(serial);
    timer = new QTimer(this);
    status = new QLabel;
    compassDial = new MinosCompass;
    selectAntenna = new QComboBox;




    ui->statusBar->addWidget(status);


    ui->verticalLayout_3->addWidget(compassDial);


    serial_connected = false;

    refreshPresetLabels();

    initActionsConnections();

    selectAntenna = ui->selectAntennaBox;
    initSelectAntennaBox();
    selectRotator->readCurrentAntenna();
    selectAntenna->setCurrentIndex(selectAntenna->findText(selectRotator->currentAntenna.name));

    openSerialPort();

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::openSerialPort()
{


    if (selectRotator->currentAntenna.name == "")
    {
        showStatusMessage("Please select an Antenna");
        return;
    }
    if(selectRotator->currentAntenna.comPort == "")
    {
        showStatusMessage("Please select a Comport");
        return;

    }
    if (selectRotator->currentAntenna.rotator.protocol == "")
    {
        showStatusMessage("Please select a rotator protcol");
        return;
    }

    SetupDialog::Rotators p = selectRotator->getCurrentAntenna();
    serial->setPortName(p.comPort);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);




    if (serial->open(QIODevice::ReadWrite)) {
        //console->setEnabled(true);
        //console->setLocalEchoEnabled(p.localEchoEnabled);
        //ui->actionConnect->setEnabled(false);
        //ui->actionDisconnect->setEnabled(true);
        //ui->actionSetup_Rotator->setEnabled(false);
        serial_connected = true;
        timer->start(1000);             // start timer to send message to controller
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6, %7, %8")
                          .arg(p.name).arg(p.rotator.protocol).arg(p.comPort).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
    //console->setEnabled(false);
    //ui->actionConnect->setEnabled(true);
    //ui->actionDisconnect->setEnabled(false);
    //ui->actionSetup_Rotator->setEnabled(true);
    serial_connected = false;
    showStatusMessage(tr("Disconnected"));
}



void MainWindow::writeData(QByteArray data)
{
    qDebug() << " write to serial" << data;
    serial->write(data);
}

void MainWindow::readData()
{
    QByteArray data = serial->readAll();

}


void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}



void MainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}


void MainWindow::rotateToController()
{
    bool ok;
    int intBearing;

    qDebug() << "Triggered from Turnbutton";

    // get bearing from bearing line edit form
    QString bearing = ui->bearingEdit->text();

    qDebug() << "string from box " << bearing;

    intBearing = bearing.toInt(&ok, 10);
    if (intBearing >= 0 && intBearing <= 359 && ok)
    {
        if (serial_connected)
        {
        rotator->rotate_to_bearing(bearing);
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Bearing Error"), tr("Invalid Bearing\nPlease enter 0 - 359"));
    }


}




void MainWindow::initActionsConnections()
{
    //connect(ui->connectButton, SIGNAL(clicked(bool)), this, SLOT(openSerialPort()));
    connect(ui->selectAntennaBox, SIGNAL(activated(int)), this, SLOT(upDateAntenna()));
    connect(ui->turnButton, SIGNAL(clicked(bool)), this, SLOT(rotateToController()));
    connect(ui->bearingEdit, SIGNAL(returnPressed()), this, SLOT(rotateToController()));
    connect(this, SIGNAL(presetRotateTo()), this, SLOT(rotateToController()));
    connect(this, SIGNAL(presetRotateTo()), ui->bearingEdit, SLOT(selectAll()));
    connect(this, SIGNAL(presetRotateTo()), ui->bearingEdit, SLOT(setFocus()));
    connect(ui->turnButton, SIGNAL(clicked(bool)), ui->bearingEdit, SLOT(selectAll()));
    connect(ui->turnButton, SIGNAL(clicked(bool)), ui->bearingEdit, SLOT(setFocus()));
    connect(ui->stopButton, SIGNAL(clicked(bool)), ui->bearingEdit, SLOT(selectAll()));
    connect(ui->stopButton, SIGNAL(clicked(bool)), ui->bearingEdit, SLOT(setFocus()));
    connect(ui->bearingEdit, SIGNAL(returnPressed()), ui->bearingEdit, SLOT(setFocus()));
    connect(ui->bearingEdit, SIGNAL(returnPressed()), ui->bearingEdit, SLOT(selectAll()));
    connect(ui->stopButton, SIGNAL(clicked(bool)), rotator, SLOT(stop_rotation()));
    connect(ui->rot_right_button, SIGNAL(clicked(bool)), this, SLOT(rotateCW()));
    connect(ui->rot_left_button, SIGNAL(clicked(bool)), this, SLOT(rotateCCW()));
    connect(this, SIGNAL(escapePressed()), rotator, SLOT(stop_rotation()));

    connect(this, SIGNAL(sendBackBearing(QString)), ui->backBearingDisplay, SLOT(setText(const QString &)));

    //connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    //connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionSetup_Comport, SIGNAL(triggered()), selectRotator, SLOT(show()));
    connect(ui->actionEdit_Presets, SIGNAL(triggered()), editPresets, SLOT(loadPresetEditFieldsShow()));
    connect(editPresets, SIGNAL(showEditPresetDialog()), editPresets, SLOT(show()));
    connect(editPresets, SIGNAL(updatePresetButtonLabels()), this, SLOT(updatePresetLabels()));
    connect(rotator, SIGNAL(bearing_updated(QString)), ui->bearingDisplay, SLOT(setText(const QString &)));
    connect(rotator, SIGNAL(bearing_updated(QString)), this, SLOT(displayBackBearing(const QString)));
    connect(rotator, SIGNAL(bearing_updated(QString)), compassDial, SLOT(compassDialUpdate(const QString &)));
    connect(rotator, SIGNAL(send_bearing_msg(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(rotator, SIGNAL(send_rotate_to_msg(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(rotator, SIGNAL(send_stop_msg(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(rotator, SIGNAL(send_az_cw_msg(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(rotator, SIGNAL(send_az_ccw_msg(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(serial, SIGNAL(readyRead()), rotator, SLOT(received_bearing()));


    connect(timer, SIGNAL(timeout()), this, SLOT(request_bearing()));
    //connect(ui->actionClear, SIGNAL(triggered()), console, SLOT(clear()));
    //connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    //connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        emit escapePressed();
    break;
    default:
        break;
    }
}

void MainWindow::request_bearing()
{
    if (serial_connected)
    {
        rotator->request_bearing();
    }
}


void MainWindow::displayBackBearing(const QString bearing)
{
    bool ok;
    int backBearing = bearing.toInt(&ok, 10);
    if (ok)
    {
        backBearing += 180;
        if (backBearing >= 360)
        {
            backBearing -= 360;
        }
        // send backBearing to display
        backBearingmsg = QString::number(backBearing, 10);
        emit sendBackBearing(backBearingmsg);
    }
}

void MainWindow::readPresets()
{
    QSettings config("./Configuration/MinosRotatorConfig.ini", QSettings::IniFormat);
    config.beginGroup("Presets");

    for (int i = 0; i < NUM_PRESETS; i++)
    {
        presetName[i] = config.value("preset" +  QString::number(i+1)).toString();
        presetBearing[i] = config.value("bearing" +  QString::number(i+1)).toString();

    }

    config.endGroup();
}




void MainWindow::refreshPresetLabels()
{

    readPresets();

    for (int i = 0; i < NUM_PRESETS; i++)
    {
        if (presetName[i] != "" || presetName[i] != presetButtons[i]->text())
        {
            presetButtons[i]->setText(presetName[i]);
        }
    }


}


void MainWindow::updatePresetLabels()
{
    refreshPresetLabels();
    update();
}



void MainWindow::clickedPreset(int buttonNumber)
{

    if (presetName[buttonNumber] != "")
    {
        if (presetBearing[buttonNumber] != "")
        {
           ui->bearingEdit->setText(presetBearing[buttonNumber]);
           emit presetRotateTo();
        }
    }

}


void MainWindow::initSelectAntennaBox()
{

    selectAntenna->addItem("");
    for (int i= 0; i < NUM_ANTENNAS; i++)
    {

        selectAntenna->addItem(selectRotator->availAntennas[i].name);
    }


}


void MainWindow::upDateAntenna()
{
    selectRotator->currentAntenna.name = ui->selectAntennaBox->currentText();
    selectRotator->currentAntenna.rotator.protocol = selectRotator->getRotatorProtocol(ui->selectAntennaBox->currentText());
    selectRotator->currentAntenna.rotator.id = selectRotator->getRotatorId(selectRotator->currentAntenna.rotator.protocol);
    selectRotator->currentAntenna.comPort = selectRotator->getRotatorComPort(ui->selectAntennaBox->currentText());
    selectRotator->saveCurrentAntenna();

    if (serial_connected)
   {
            closeSerialPort();
   }

   openSerialPort();

}


void MainWindow::rotateCW()
{
    if (serial_connected)
    {
        rotator->rotateClockwise();
    }
}



void MainWindow::rotateCCW()
{
    if (serial_connected)
    {
        rotator->rotateCClockwise();
    }
}
