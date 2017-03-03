#include "base_pch.h"

#include "RPCCommandConstants.h"
#include "rotatorlogic.h"

#include "rotatormainwindow.h"
#include "ui_rotatormainwindow.h"


RotatorMainWindow *MinosRotatorForm;

RotatorMainWindow::RotatorMainWindow(QWidget *parent) :
    QMainWindow(parent), rl(0),
    ui(new Ui::RotatorMainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    enableTrace( "./TraceLog", "MinosChat_" );

    createCloseEvent();
    QSettings settings;
    QByteArray geometry = settings.value("geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);

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
    // and tidy up all loose ends
    MinosRPCObj::clearRPCObjects();
    XMPPClosedown();
    LogTimerTimer( );

    QWidget::closeEvent(event);
}
void RotatorMainWindow::moveEvent(QMoveEvent * event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    QWidget::moveEvent(event);
}
void RotatorMainWindow::resizeEvent(QResizeEvent * event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
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
    case rpcConstants::eRotateLeft:
        sdir = "L/";
    break;
    case rpcConstants::eRotateDirect:
        sdir = "D/";
    break;
    case rpcConstants::eRotateRight:
        sdir = "R/";
    break;
    case rpcConstants::eRotateStop:
        sdir = "S/";
    break;
    }

    rl->publishState(sdir + ui->angle->text());
}
