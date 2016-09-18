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

    QTimer::singleShot(100, this, setPosition);
}
void windowMonitor::setPosition()
{
    QPoint bl = controlMain->frameGeometry().bottomLeft();
    move(bl);
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

void windowMonitor::checkPTT(bool checked)
{
    ui->PTTInCheckBox->setChecked(checked);
}

