#include "logger_pch.h"
#include "locTreeFrame.h"
#include "ui_locTreeFrame.h"
#include "htmldelegate.h"

static QString lConv(const QString &tlsq, int col, int row)
{
   char cvxl = tlsq[0].toLatin1();
   char cvyl = tlsq[1].toLatin1();
   char cvxn = tlsq[2].toLatin1();
   char cvyn = tlsq[3].toLatin1();

   for ( int i = 0; i < col; i++ )
   {
      char cv = ++cvxn;

      if ( cv > '9' )
      {
         cvxn = '0';
         ++cvxl;
      }
   }

   for ( int i = 0; i < row; i++ )
   {
      char cv = --cvyn;

      if ( cv < '0' )
      {
         cvyn = '9';
         --cvyl;
      }
   }

   QString res = QString(cvxl) + cvyl + cvxn + cvyn;
   return res;
}
static QString l_add(const QString &sq, int x, int y)
{
   char cvxl = sq[0].toLatin1();
   char cvyl = sq[1].toLatin1();
   char cvxn = sq[2].toLatin1();
   char cvyn = sq[3].toLatin1();

   for ( int i = 0; i < x; i++ )
   {
      char cv = ++cvxn;

      if ( cv > '9' )
      {
         cvxn = '0';
         ++cvxl;
      }
   }

   for ( int i = 0; i < y; i++ )
   {
      char cv = ++cvyn;

      if ( cv > '9' )
      {
         cvyn = '0';
         ++cvyl;
      }
   }

   QString res = QString(cvxl) + cvyl + cvxn + cvyn;
   return res;
}
static QString l_sub(const QString &sq, int x, int y)
{
    char cvxl = sq[0].toLatin1();
    char cvyl = sq[1].toLatin1();
    char cvxn = sq[2].toLatin1();
    char cvyn = sq[3].toLatin1();

    for ( int i = 0; i < x; i++ )
    {
      char cv = --cvxn;

      if ( cv < '0' )
      {
         cvxn = '9';
         --cvxl;
      }
   }

   for ( int i = 0; i < y; i++ )
   {
      char cv = --cvyn;

      if ( cv < '0' )
      {
            cvyn= '9';
            --cvyl;
        }
    }

   QString res = QString(cvxl) + cvyl + cvxn + cvyn;
   return res;
}


LocTreeFrame::LocTreeFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::LocTreeFrame), ct(0)
{
    ui->setupUi(this);

    ui->LocTree->setItemDelegate(new HtmlDelegate);
}
LocTreeFrame::~LocTreeFrame()
{
    delete ui;
    ct = nullptr;
}
void LocTreeFrame::setContest(BaseContestLog *contest)
{
    ct = contest;

    reInitialiseLocators();
}

void LocTreeFrame::reInitialiseLocators()
{
    ui->LocTree->clear();

    //initialise these to a range round the contest location
    QString ctLoc = ct->myloc.loc.getValue();

    QString ctLocN = l_add(ctLoc, 0, 5);
    QString NLoc = QString(ctLocN[1]) + ctLocN[3];

    QString ctLocS = l_sub(ctLoc, 0, 5);
    QString SLoc = QString(ctLocS[1]) + ctLocS[3];

    QString ctLocW = l_sub(ctLoc, 3, 0);
    QString WLoc = QString(ctLocW[0]) + ctLocW[2];

    QString ctLocE = l_add(ctLoc, 3, 0);
    QString ELoc = QString(ctLocE[0]) + ctLocE[2];

    for (int k = 0; k < ct->locs.llist.size(); k++)
    {
        QTreeWidgetItem *it = new QTreeWidgetItem(ui->LocTree);
        QString locStart = ct->locs.itemAt(k) ->loc;
        it->setText(0, locStart);
        it->setExpanded(true);

        for (int j = 0; j < 10; j++)
        {
            QString dispLine;
            for (int i = 0; i < 10; i++)
            {
                LocCount *lc = ct->locs.itemAt(k) ->map( j * 10 + i );
                QString disp = QString("%1").arg(j * 10 + i, 2, 10, QChar('0'));

                if ( lc && (lc->UKLocCount || lc->nonUKLocCount))
                {
                    if (ct->usesBonus.getValue())
                    {
                        QColor multhighlight = Qt::black;
                        switch (ct->getSquareBonus(locStart + disp))
                        {
                        case 500:  //blue
                            multhighlight = Qt::blue;
                            break;
                        case 1000: //green
                            multhighlight = Qt::darkGreen;
                            break;
                        case 2000: //red
                            multhighlight = Qt::red;
                            break;
                        }

                        dispLine += HtmlFontColour(multhighlight) + "<b>" + disp + "</b>" + " (" + QString::number(lc->UKLocCount + lc->nonUKLocCount) + ") ";
                    }
                    else
                    {
                        dispLine += disp + " (" + QString::number(lc->UKLocCount)
                                    + (lc->nonUKLocCount?("/" + QString::number(lc->nonUKLocCount)):QString("")) + ") ";
                    }

                    QString col = QString(locStart[0]) + disp[0];
                    QString row = QString(locStart[1]) + disp[1];

                    QString loc = locStart + disp;

                    if (row < SLoc || SLoc.isEmpty())
                        SLoc = row;

                    if (row > NLoc || NLoc.isEmpty())
                        NLoc = row;

                    if (col < WLoc || WLoc.isEmpty())
                        WLoc = col;

                    if (col > ELoc || ELoc.isEmpty())
                        ELoc = col;
                }
            }
            if (dispLine.size())
            {
                QTreeWidgetItem *treeItem = new QTreeWidgetItem();

                treeItem->setText(0, dispLine);

                it->addChild(treeItem);
            }
        }
    }
}

