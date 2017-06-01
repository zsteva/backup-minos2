#ifndef MINOSTABLEVIEW_H
#define MINOSTABLEVIEW_H
#include <QObject>
#include <QTableView>

class MinosTableView:public QTableView
{
    Q_OBJECT
    QPersistentModelIndex centreCell;
public:
    MinosTableView(QWidget *parent);
    void scrollTo(const QModelIndex &index, ScrollHint hint);

protected:
    virtual void scrollContentsBy(int x, int y) override;
    void showEvent(QShowEvent *) override;


signals:
    void minosViewScrolled();

};

#endif // MINOSTABLEVIEW_H
