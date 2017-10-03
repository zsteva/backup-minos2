#ifndef CONFIGELEMENTFRAME_H
#define CONFIGELEMENTFRAME_H

#include <QFrame>
#include "ConfigFile.h"

namespace Ui {
class ConfigElementFrame;
}

class ConfigElementFrame : public QFrame
{
    Q_OBJECT

    QSharedPointer<RunConfigElement> configElement;
    void checkEnabled();

public:
    explicit ConfigElementFrame(QWidget *parent = 0);
    ~ConfigElementFrame();

    void setElement(QSharedPointer<RunConfigElement> configElement);
    bool saveElement();
    void setNameFocus();

private slots:
    void on_programBrowseButton_clicked();

    void on_homeDirectoryBrowse_clicked();

    void on_deleteButton_clicked();

    void on_rbRunLocally_clicked();

    void on_rbConnectRemote_clicked();

    void on_rbConnectLocal_clicked();

    void on_appTypeCombo_currentIndexChanged(const QString &arg1);

    void on_advancedCheckbox_clicked();

    void on_enabledCheckbox_clicked();

private:
    Ui::ConfigElementFrame *ui;
};

#endif // CONFIGELEMENTFRAME_H
