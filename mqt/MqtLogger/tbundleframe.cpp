#include "tbundleframe.h"
#include "ui_tbundleframe.h"

TBundleFrame::TBundleFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TBundleFrame)
{
    ui->setupUi(this);
}

TBundleFrame::~TBundleFrame()
{
    delete ui;
}
