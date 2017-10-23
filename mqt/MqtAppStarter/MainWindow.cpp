#include "base_pch.h"
#include "ConfigFile.h"
#include "StartConfig.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createCloseEvent();
    QSettings settings;
    QByteArray geometry = settings.value("geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);

    if (MinosConfig::getMinosConfig() ->getAutoStart())
    {
        connect(&startTimer, SIGNAL(timeout()), this, SLOT(startTimer_Timeout()));
        startTimer.start(100);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
}
void MainWindow::moveEvent(QMoveEvent *event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    QMainWindow::moveEvent(event);
}
void MainWindow::resizeEvent(QResizeEvent * event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    QMainWindow::resizeEvent(event);
}
void MainWindow::changeEvent( QEvent* e )
{
    if( e->type() == QEvent::WindowStateChange )
    {
        QSettings settings;
        settings.setValue("geometry", saveGeometry());
    }
}

void MainWindow::on_appsButton_clicked()
{

    StartConfig startConfig(this, true);
    startConfig.exec();
}

void MainWindow::on_closeButton_clicked()
{
    close();
}
void MainWindow::startTimer_Timeout()
{
    startTimer.stop();
    start();
}

void MainWindow::start()
{
    MinosConfig::getMinosConfig() ->start();
}

