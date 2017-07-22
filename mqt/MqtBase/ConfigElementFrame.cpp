#include "base_pch.h"

#include <QFileDialog>

#include "ConfigElementFrame.h"
#include "ui_ConfigElementFrame.h"
#include "ConfigElementFrame.h"

ConfigElementFrame::ConfigElementFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ConfigElementFrame)
{
    ui->setupUi(this);

    QStringList appTypes = MinosConfig::getAppTypes();
    ui->appTypeCombo->addItems(appTypes);
}

ConfigElementFrame::~ConfigElementFrame()
{
    delete ui;
}

void ConfigElementFrame::setElement(QSharedPointer<TConfigElement> c)
{
    this->c = c;

    if (c->runType == rtNone)
        ui->rbNoAction->setChecked(true);
    if (c->runType == rtRunLocal)
        ui->rbRunLocally->setChecked(true);
    if (c->runType == rtConnectServer)
        ui->rbConnectRemote->setChecked(true);

    QStringList appTypes = MinosConfig::getAppTypes();
    QString at = appTypes[c->appType];
    ui->appTypeCombo->setCurrentText(at);

    ui->elementNameEdit->setText(c->name);
    ui->programNameEdit->setText(c->commandLine);
    ui->homeDirectoryEdit->setText(c->rundir);
    ui->parametersEdit->setText(c->params);
    ui->serverNameEdit->setText(c->server);

    checkEnabled();
}
void ConfigElementFrame::setNameFocus()
{
    ui->elementNameEdit->setFocus();
}

bool ConfigElementFrame::saveElement()
{
    if (ui->rbNoAction->isChecked())
        c->runType = rtNone;
    if (ui->rbRunLocally->isChecked())
        c->runType = rtRunLocal;
    if (ui->rbConnectLocal->isChecked())
        c->runType = rtConnectLocal;
    if (ui->rbConnectRemote->isChecked())
        c->runType = rtConnectServer;

    QString S = ui->appTypeCombo->currentText();
    QStringList appTypes = MinosConfig::getAppTypes();

    for (int i = 0; i < atMax; i++)
    {
        if (S.compare(appTypes[i], Qt::CaseInsensitive) == 0)
        {
            c->appType = static_cast<AppType>(i);
        }
    }

    c->name = ui->elementNameEdit->text();
    c->rundir = ui->homeDirectoryEdit->text();
    c->commandLine = ui->programNameEdit->text();
    c->params = ui->parametersEdit->text();
    c->server = ui->serverNameEdit->text();

    return true;
}
void ConfigElementFrame::on_programBrowseButton_clicked()
{
    // We need
    QDir cdir(GetCurrentDir());
    QString InitialDir = ExtractFileDir(ui->programNameEdit->text());

    // how do we search for executable files on Linux?
    // This MIGHT work
    QString Filter = "Executable Files (*.exe);;"
                     "All Files (*.*)" ;

    QFileDialog dialog(this, "Minos 2 Component Program", InitialDir);
    const QStringList schemes = QStringList(QStringLiteral("file"));

    dialog.setSupportedSchemes(schemes);
    dialog.setNameFilter(Filter);
    dialog.setFilter(QDir::Files | QDir::Executable);
    dialog.setFileMode(QFileDialog::ExistingFile);

    if (dialog.exec() == QDialog::Accepted)
    {
        // need to make path relative
        QString progName = dialog.selectedUrls().value(0).toLocalFile();
        QString rpath = cdir.relativeFilePath(progName);
        ui->programNameEdit->setText(rpath);

    }

}

void ConfigElementFrame::on_homeDirectoryBrowse_clicked()
{
    QDir cdir(GetCurrentDir());

    QString fpath =  ui->homeDirectoryEdit->text();

    QString destDir = QFileDialog::getExistingDirectory(
                  0,
                  "Set Working Directory",
                  fpath,
                  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                   );
    if (!destDir.isEmpty())
    {
        QString rpath = cdir.relativeFilePath(destDir);
        ui->homeDirectoryEdit->setText(rpath);
    }
}

void ConfigElementFrame::on_deleteButton_clicked()
{
    // mark the element as deleted
    ui->elementNameEdit->setText("<Deleted>");
}

void ConfigElementFrame::checkEnabled()
{
    if (ui->rbNoAction->isChecked())
    {
        ui->programFrame->setEnabled(false);
        ui->serverFrame->setEnabled(false);
    }
    else if (ui->rbRunLocally->isChecked())
    {
        ui->programFrame->setEnabled(true);
        ui->serverFrame->setEnabled(false);
    }
    else if (ui->rbConnectLocal->isChecked())
    {
        ui->programFrame->setEnabled(false);
        ui->serverFrame->setEnabled(false);
    }
    else if (ui->rbConnectRemote->isChecked())
    {
        ui->programFrame->setEnabled(false);
        ui->serverFrame->setEnabled(true);
    }
}

void ConfigElementFrame::on_rbNoAction_clicked()
{
    checkEnabled();
}

void ConfigElementFrame::on_rbRunLocally_clicked()
{
    checkEnabled();
}

void ConfigElementFrame::on_rbConnectRemote_clicked()
{
    checkEnabled();
}

void ConfigElementFrame::on_rbConnectLocal_clicked()
{
    checkEnabled();
}
