#include "qsologframe.h"
#include "ui_qsologframe.h"

QSOLogFrame::QSOLogFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::QSOLogFrame)
{
    ui->setupUi(this);
}

QSOLogFrame::~QSOLogFrame()
{
    delete ui;
}
