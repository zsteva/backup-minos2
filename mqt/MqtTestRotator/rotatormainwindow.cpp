#include "base_pch.h"

#include "RotatorConstants.h"
#include "rotatorlogic.h"

#include "rotatormainwindow.h"
#include "ui_rotatormainwindow.h"


RotatorMainWindow *MinosRotatorForm;

RotatorMainWindow::RotatorMainWindow(QWidget *parent) :
    QMainWindow(parent), rl(0),
    ui(new Ui::RotatorMainWindow)
{
    ui->setupUi(this);

    enableTrace( "./TraceLog", "MinosChat_" );

    createCloseEvent();

    MinosRotatorForm = this;

    connect(&LogTimer, SIGNAL(timeout()), this, SLOT(LogTimerTimer()));
    LogTimer.start(100);

    rl = new RotatorLogic(this);

    connect(rl, SIGNAL(setRotation(int,int)), this, SLOT(onSetRotation(int,int)));
}

RotatorMainWindow::~RotatorMainWindow()
{
    delete rl;
    delete ui;
}
void RotatorMainWindow::logMessage( QString s )
{
   trace( s );
}

void RotatorMainWindow::closeEvent(QCloseEvent *event)
{
    MinosRPCObj::clearRPCObjects();
    XMPPClosedown();
    LogTimerTimer( );
    // and tidy up all loose ends

    /*
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    */
    QWidget::closeEvent(event);
}
void RotatorMainWindow::resizeEvent(QResizeEvent * event)
{
    /*
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    */
    QWidget::resizeEvent(event);
}

void RotatorMainWindow::LogTimerTimer(  )
{

   static bool closed = false;
   if ( !closed )
   {
      if ( checkCloseEvent() )
      {
         closed = true;
         close();
      }
   }
}

void RotatorMainWindow::onSetRotation(int direction, int angle)
{
    ui->direction->setText( QString::number(direction));
    ui->angle->setText( QString::number(angle));
}


void RotatorMainWindow::on_pushButton_2_clicked()
{
    close();
}

void RotatorMainWindow::on_pushButton_clicked()
{
    QString sdir;
    switch (ui->direction->text().toInt())
    {
    case eRotateLeft:
        sdir = "L/";
    break;
    case eRotateDirect:
        sdir = "D/";
    break;
    case eRotateRight:
        sdir = "R/";
    break;
    case eRotateStop:
        sdir = "S/";
    break;
    }

    rl->publishState(sdir + ui->angle->text());
}