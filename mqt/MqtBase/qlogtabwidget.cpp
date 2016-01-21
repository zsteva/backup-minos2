#include "qlogtabwidget.h"

QLogTabWidget::QLogTabWidget(QWidget *parent) : QTabWidget(parent)
{
    setParent(parent);
}

//Overridden method from QTabWidget
QTabBar* QLogTabWidget::tabBar()
{
    return QTabWidget::tabBar();
}
void QLogTabWidget::setTabColor(int index, QColor colour)
{
    this->tabBar()->setTabTextColor(index, colour);
}
