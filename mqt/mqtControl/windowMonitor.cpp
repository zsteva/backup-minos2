#include "windowMonitor.h"
#include "ui_windowMonitor.h"
#include "ControlMain.h"
extern ControlMain *controlMain;

windowMonitor::windowMonitor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::windowMonitor)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    setPosTimer.setSingleShot(true);
    connect(&setPosTimer, SIGNAL(timeout()), this, SLOT(setPosition()));
    setPosTimer.start(100);
}
void windowMonitor::setPosition()
{
    QPoint tl = controlMain->frameGeometry().topLeft();
    tl.setY(tl.y() + controlMain->frameGeometry().height() + 30);
    move(tl);
}

windowMonitor::~windowMonitor()
{
    delete ui;
}

bool windowMonitor::L1Checked()
{
    return ui->L1CheckBox->isChecked();
}

bool windowMonitor::L2Checked()
{
    return ui->L2CheckBox->isChecked();
}

bool windowMonitor::L3Checked()
{
    return ui->L3CheckBox->isChecked();
}

bool windowMonitor::L4Checked()
{
    return ui->L4CheckBox->isChecked();
}

bool windowMonitor::PTTChecked()
{
    return ui->PTTInCheckBox->isChecked();
}

void windowMonitor::checkL1(bool checked)
{
    ui->L1CheckBox->setChecked(checked);
}

void windowMonitor::checkL2(bool checked)
{
    ui->L2CheckBox->setChecked(checked);
}

void windowMonitor::checkL3(bool checked)
{
    ui->L3CheckBox->setChecked(checked);
}

void windowMonitor::checkL4(bool checked)
{
    ui->L4CheckBox->setChecked(checked);
}

void windowMonitor::checkPTT(bool checked)
{
    ui->PTTInCheckBox->setChecked(checked);
}

