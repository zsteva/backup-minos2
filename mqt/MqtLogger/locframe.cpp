#include "logger_pch.h"
#include "locframe.h"
#include "ui_locframe.h"

LocFrame::LocFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::LocFrame)
{
    ui->setupUi(this);
}

LocFrame::~LocFrame()
{
    delete ui;
}
void LocFrame::setContest(BaseContestLog *contest)
{
    ct = contest;
    reInitialiseLocators();
}

void LocFrame::reInitialiseLocators()
{
    ui->LocTree->clear();
    for (int k = 0; k <(int) ct->locs.llist.size(); k++)
    {
        QTreeWidgetItem *it = new QTreeWidgetItem(ui->LocTree);
        it->setText(0, ct->locs.llist[ k ] ->loc);
        it->setExpanded(true);

        for (int j = 0; j < 10; j++)
        {
            QString dispLine;
            for (int i = 0; i < 10; i++)
            {
                LocCount *lc = ct->locs.llist[ k ] ->map( j * 10 + i );
                QString disp = QString("%1").arg(j * 10 + i, 2, 10, QChar('0'));

                if ( lc && (lc->UKLocCount || lc->nonUKLocCount))
                    dispLine += disp + " (" + QString::number(lc->UKLocCount) + (lc->nonUKLocCount?("/" + QString::number(lc->nonUKLocCount)):QString("")) + ") ";
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
