#include "MinosTableView.h"

MinosTableView::MinosTableView(QWidget *parent):QTableView(parent)
{

}

void MinosTableView::scrollContentsBy(int dx, int dy)
{
    QTableView::scrollContentsBy(dx, dy);
    emit minosViewScrolled();
}

