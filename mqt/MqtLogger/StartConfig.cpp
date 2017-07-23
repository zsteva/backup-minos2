#include "base_pch.h"
#include "StartConfig.h"
#include "ui_StartConfig.h"

static void configCloseCallback(QWidget *w)
{
    w->close();
}

StartConfig::StartConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartConfig)
{
    ui->setupUi(this);
    ui->configFrame->initialise(this, &::configCloseCallback, false);

    QSettings settings;
    QByteArray geometry = settings.value("starConfigGeometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);
}

StartConfig::~StartConfig()
{
    delete ui;
}

void StartConfig::moveEvent(QMoveEvent *event)
{
    QSettings settings;
    settings.setValue("starConfigGeometry", saveGeometry());
    QDialog::moveEvent(event);
}
void StartConfig::resizeEvent(QResizeEvent * event)
{
    QSettings settings;
    settings.setValue("starConfigGeometry", saveGeometry());
    QDialog::resizeEvent(event);
}
void StartConfig::changeEvent( QEvent* e )
{
    if( e->type() == QEvent::WindowStateChange )
    {
        QSettings settings;
        settings.setValue("starConfigGeometry", saveGeometry());
    }
}
