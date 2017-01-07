#include "logger_pch.h"

#include "TSessionManager.h"
#include "ui_TSessionManager.h"

TSessionManager::TSessionManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TSessionManager)
{
    ui->setupUi(this);
    QSettings settings;
    QByteArray geometry = settings.value("TSessionManager/geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);
}

TSessionManager::~TSessionManager()
{
    delete ui;
}

void TSessionManager::on_OKButton_clicked()
{
    accept();
}

void TSessionManager::on_cancelButton_clicked()
{
    reject();
}
void TSessionManager::doCloseEvent()
{
    QSettings settings;
    settings.setValue("TSessionManager/geometry", saveGeometry());
}
void TSessionManager::reject()
{
    doCloseEvent();
    QDialog::reject();
}
void TSessionManager::accept()
{
    doCloseEvent();
    QDialog::accept();
}

