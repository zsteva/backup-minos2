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
    this->configElement = c;

    ui->enabledCheckbox->setChecked(c->enabled);

    if (c->runType == RunLocal)
        ui->rbRunLocally->setChecked(true);
    else if (c->runType == ConnectLocal)
        ui->rbConnectLocal->setChecked(true);
    else if (c->runType == ConnectServer)
        ui->rbConnectRemote->setChecked(true);
    else
        ui->rbRunLocally->setChecked(true);


    QString at = c->appType;
    ui->appTypeCombo->setCurrentText(at);

    ui->elementNameEdit->setText(c->name);
    ui->programNameEdit->setText(c->commandLine);
    ui->homeDirectoryEdit->setText(c->rundir);
    ui->parametersEdit->setText(c->params);
    ui->serverNameEdit->setText(c->server);
    ui->remoteAppNameEdit->setText(c->remoteApp);

    ui->advancedCheckbox->setChecked(c->showAdvanced);
    ui->enabledCheckbox->setChecked(c->enabled);

    checkEnabled();
}
void ConfigElementFrame::setNameFocus()
{
    ui->elementNameEdit->setFocus();
}

bool ConfigElementFrame::saveElement()
{
    if (ui->rbRunLocally->isChecked())
        configElement->runType = RunLocal;
    if (ui->rbConnectLocal->isChecked())
        configElement->runType = ConnectLocal;
    if (ui->rbConnectRemote->isChecked())
        configElement->runType = ConnectServer;

    configElement->showAdvanced = ui->advancedCheckbox->isChecked();
    configElement->enabled = ui->enabledCheckbox->isChecked();

    configElement->appType = ui->appTypeCombo->currentText();

    configElement->name = ui->elementNameEdit->text().trimmed();
    configElement->rundir = ui->homeDirectoryEdit->text().trimmed();
    configElement->commandLine = ui->programNameEdit->text().trimmed();
    configElement->params = ui->parametersEdit->text().trimmed();
    configElement->server = ui->serverNameEdit->text().trimmed();
    configElement->remoteApp = ui->remoteAppNameEdit->text().trimmed();

    return true;
}
void ConfigElementFrame::on_programBrowseButton_clicked()
{
    // We need
    QDir cdir(GetCurrentDir());
    QString InitialDir = ExtractFileDir(ui->programNameEdit->text());

    QFileDialog dialog(this, "Minos Component Program", InitialDir);
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
    ui->enabledCheckbox->setChecked(false);
}

void ConfigElementFrame::checkEnabled()
{
    if (!ui->enabledCheckbox->isChecked())
    {
        ui->programFrame->setEnabled(false);
        ui->serverFrame->setEnabled(false);
        ui->actionsGroup->setEnabled(false);
    }
    else if (ui->rbRunLocally->isChecked())
    {
        ui->programFrame->setEnabled(true);
        ui->serverFrame->setEnabled(false);
        ui->actionsGroup->setEnabled(true);
    }
    else if (ui->rbConnectLocal->isChecked())
    {
        ui->programFrame->setEnabled(false);
        ui->serverFrame->setEnabled(false);
        ui->actionsGroup->setEnabled(true);
    }
    else if (ui->rbConnectRemote->isChecked())
    {
        ui->programFrame->setEnabled(false);
        ui->serverFrame->setEnabled(true);
        ui->actionsGroup->setEnabled(true);
    }

    ui->advancedGroup->setVisible( ui->advancedCheckbox->isChecked());
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

    if (ui->remoteAppNameEdit->text().isEmpty())
        ui->remoteAppNameEdit->setText(value);

    if (ui->programNameEdit->text().isEmpty())
    {
        AppConfigElement ace = MinosConfig::getMinosConfig()->getAppConfigElement(value);
        ui->programNameEdit->setText(ace.appPath);
    }

    if (ui->homeDirectoryEdit->text().isEmpty())
        ui->homeDirectoryEdit->setText(".");
}

void ConfigElementFrame::on_advancedCheckbox_clicked()
{
    checkEnabled();
}

void ConfigElementFrame::on_enabledCheckbox_clicked()
{
    checkEnabled();
}
