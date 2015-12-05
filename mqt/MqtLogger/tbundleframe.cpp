#include "logger_pch.h"

#include "tbundleframe.h"
#include "ui_tbundleframe.h"

TBundleFrame::TBundleFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TBundleFrame)
  , bundle( 0 ), bname( 0 )
{
    ui->setupUi(this);
}

TBundleFrame::~TBundleFrame()
{
    delete ui;
}

void TBundleFrame::initialise()
{
   QString csect = bundle->getSection();
   QStringList slist = bundle->getSections( );
   ui->BundleSection->clear();

   int offset = -1;
   for ( int i = 0; i < slist.count(); i++ )
   {
      if ( csect == slist[ i ] )
      {
         offset = i;
      }
      ui->BundleSection->addItem( slist[ i ] );
   }
   if ( offset >= 0 )
   {
      ui->BundleSection->setCurrentIndex( offset);
   }
   else
   {
      ui->BundleSection->setCurrentIndex( 0);
   }
}
void TBundleFrame::initialise(const QString &cap, SettingsBundle *b, MinosItem<QString> *name )
{
   if ( b )
   {
      bundle = b;
      bname = name;
      // here we need to read in the histories
      ui->BundleNameLabel->setText(cap);
      initialise();
   }
}
//---------------------------------------------------------------------------
bool TBundleFrame::doEdit( )
{/*
   TSettingsEditDlg ed ( this, bundle );
   if ( ed->exec() == QDialog::Accepted )
   {
      initialise();
      return true;
   }
   */
   return false;
}
//---------------------------------------------------------------------------
void TBundleFrame::enableBundle( bool state )
{
   ui->BundleNameLabel->setEnabled(state);
   ui->BundleSection->setEnabled(state);
   ui->BundleEdit->setEnabled(state);
}
//---------------------------------------------------------------------------
void TBundleFrame::BundleSectionChange(  )
{
   if ( bundle )
   {
      bundle->openSection( ui->BundleSection->currentText() );
      bname->setValue( bundle->getSection() );
   }
}
//---------------------------------------------------------------------------

void TBundleFrame::BundleEditClick( )
{
   doEdit( );
}
//---------------------------------------------------------------------------
