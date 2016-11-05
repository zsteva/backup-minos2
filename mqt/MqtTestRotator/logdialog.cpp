#include "logdialog.h"
#include "ui_logdialog.h"
#include <QFile>
#include <QDateTime>

LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);
}

LogDialog::~LogDialog()
{
    delete ui;
}
