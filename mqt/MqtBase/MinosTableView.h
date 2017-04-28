#ifndef MINOSTABLEVIEW_H
#define MINOSTABLEVIEW_H
#include <QObject>
#include <QTableView>

class MinosTableView:public QTableView
{
    Q_OBJECT
public:
    MinosTableView(QWidget *parent);
protected:
    virtual void scrollContentsBy(int x, int y) override;

signals:
    void minosViewScrolled();

};

#endif // MINOSTABLEVIEW_H
