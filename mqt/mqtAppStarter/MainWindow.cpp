#include "MainWindow.h"
#include "ui_MainWindow.h"


void closeCallback(QWidget *w)
{
    w->close();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->configFrame->initialise(this, &::closeCallback);
}

MainWindow::~MainWindow()
{
    delete ui;
}
