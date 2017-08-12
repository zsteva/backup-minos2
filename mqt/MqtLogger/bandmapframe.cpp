#include "bandmapframe.h"
#include "ui_bandmapframe.h"
#include "rotatorCommonConstants.h"

BandMapFrame::BandMapFrame(QWidget *parent):
    QFrame(parent)
    , ui(new Ui::BandMapFrame)
    , bandMapLoaded(false)
{
    ui->setupUi(this);
}


BandMapFrame::~BandMapFrame()
{
    delete ui;
}
