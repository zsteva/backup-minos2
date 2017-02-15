#include "logger_pch.h"
#include "locframe.h"
#include "ui_locframe.h"
#include "htmldelegate.h"


LocFrame::LocFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::LocFrame), ct(0)
{
    ui->setupUi(this);

    ui->LocTree->setItemDelegate(new HtmlDelegate);
}

LocFrame::~LocFrame()
{
    delete ui;
    ct = nullptr;
}
void LocFrame::setContest(BaseContestLog *contest)
{
    ct = contest;
    reInitialiseLocators();
}

void LocFrame::reInitialiseLocators()
{
    ui->LocTree->clear();
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
                    if (ct->UKACBonus.getValue())
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

