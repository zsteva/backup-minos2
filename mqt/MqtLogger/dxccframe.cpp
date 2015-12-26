#include "dxccframe.h"
#include "ui_dxccframe.h"

DXCCFrame::DXCCFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::DXCCFrame)
{
    ui->setupUi(this);
}

DXCCFrame::~DXCCFrame()
{
    delete ui;
}
void DXCCFrame::reInitialiseCountries()
{
    /*
   CountryMultTree->RootNodeCount = 0;
   if ( !ct )
   {
      // clear down
      return ;
   }

   CountryMultTree->RootNodeCount = MultLists::getMultLists() ->getCtryListSize();
   CountryMultTree->ValidateNode( 0, true );
   */


}
