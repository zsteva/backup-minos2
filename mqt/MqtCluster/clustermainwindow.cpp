#include "clustermainwindow.h"
#include "ui_clustermainwindow.h"

ClusterMainWindow::ClusterMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClusterMainWindow)
{
    ui->setupUi(this);
}

ClusterMainWindow::~ClusterMainWindow()
{
    delete ui;
}
