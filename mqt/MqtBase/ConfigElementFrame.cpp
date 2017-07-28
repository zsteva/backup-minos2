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

    QStringList appTypes = MinosConfig::getMinosConfig()->getAppTypes();
    ui->appTypeCombo->addItems(appTypes);
}

ConfigElementFrame::~ConfigElementFrame()
{
    delete ui;
}

void ConfigElementFrame::setElement(QSharedPointer<RunConfigElement> c)
{
    this->c = c;

    if (c->runType.isEmpty())
        ui->rbNoAction->setChecked(true);
    if (c->runType == "RunLocal")
        ui->rbRunLocally->setChecked(true);
    if (c->runType == "ConnectServer")
        ui->rbConnectRemote->setChecked(true);

    QString at = c->appType;
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
        c->runType = RunTypeNone;
    if (ui->rbRunLocally->isChecked())
        c->runType = RunLocal;
    if (ui->rbConnectLocal->isChecked())
        c->runType = ConnectLocal;
    if (ui->rbConnectRemote->isChecked())
        c->runType = ConnectServer;

    c->appType = ui->appTypeCombo->currentText();

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

    QFileDialog dialog(this, "Minos 2 Component Program", InitialDir);
#if QT_VERSION >= 0x050600
    const QStringList schemes = QStringList(QStringLiteral("file"));

    dialog.setSupportedSchemes(schemes);
#endif

#ifdef Q_OS_WIN
    QString Filter = "Executable Files (*.exe);;"
                     "All Files (*.*)" ;
    dialog.setNameFilter(Filter);
#else
    dialog.setFilter(QDir::AllDirs | QDir::Files | QDir::Dirs /*| QDir::Executable*/); //executable doesn't seem to work
#endif

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

void ConfigElementFrame::on_appTypeCombo_currentIndexChanged(const QString &value)
{
    if (ui->elementNameEdit->text().isEmpty())
        ui->elementNameEdit->setText(value);
    if (ui->programNameEdit->text().isEmpty())
        ui->programNameEdit->setText(value);
    if (ui->homeDirectoryEdit->text().isEmpty())
        ui->homeDirectoryEdit->setText(".");
}
