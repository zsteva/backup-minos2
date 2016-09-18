#include "keyerAbout.h"
#include "ui_keyerAbout.h"

keyerAbout::keyerAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::keyerAbout)
{
    ui->setupUi(this);
}

keyerAbout::~keyerAbout()
{
    delete ui;
}
