#include "base_pch.h"
#include "StartConfig.h"
#include "ui_StartConfig.h"

static void configCloseCallback(QWidget *w)
{
    w->close();
}

StartConfig::StartConfig(QWidget *parent, bool showAutoStart) :
    QDialog(parent),
    ui(new Ui::StartConfig)
{
    ui->setupUi(this);

    QSettings settings;
    QByteArray geometry = settings.value("startConfigGeometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);

    ui->configFrame->initialise(this, &::configCloseCallback, showAutoStart);
}

StartConfig::~StartConfig()
{
    delete ui;
}

void StartConfig::moveEvent(QMoveEvent *event)
{
    QSettings settings;
    settings.setValue("startConfigGeometry", saveGeometry());
    QDialog::moveEvent(event);
}
void StartConfig::resizeEvent(QResizeEvent * event)
{
    QSettings settings;
    settings.setValue("startConfigGeometry", saveGeometry());
    QDialog::resizeEvent(event);
}
void StartConfig::changeEvent( QEvent* e )
{
    if( e->type() == QEvent::WindowStateChange )
    {
        QSettings settings;
        settings.setValue("startConfigGeometry", saveGeometry());
    }
}
