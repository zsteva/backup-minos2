#include "districtframe.h"
#include "ui_districtframe.h"

DistrictFrame::DistrictFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::DistrictFrame)
{
    ui->setupUi(this);
}

DistrictFrame::~DistrictFrame()
{
    delete ui;
}
void DistrictFrame::reInitialiseDistricts()
{
    /*
   DistrictMultTree->RootNodeCount = 0;
   if ( !ct )
   {
      // clear down
      return ;
   }
   DistrictMultTree->RootNodeCount = MultLists::getMultLists() ->getDistListSize();
   DistrictMultTree->ValidateNode( 0, true );
   */
}
