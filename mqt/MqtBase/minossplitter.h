#ifndef MINOSSPLITTER_H
#define MINOSSPLITTER_H

#include <QWidget>
#include <QAction>
#include <QLinearGradient>
#include <QSplitter>
#include <QSplitterHandle>
#include <QPainter>
#include <QPaintEvent>
#include <QSizePolicy>

class MinosSplitterHandle : public QSplitterHandle
{
    Q_OBJECT
public:
    MinosSplitterHandle(Qt::Orientation orientation, QSplitter *parent);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QLinearGradient gradient;
};
//============================================================================
class MinosSplitter : public QSplitter
{
    Q_OBJECT
private:
   MinosSplitterHandle *myHandle;
public:
   MinosSplitter(QWidget *parent = 0);

protected:
   QSplitterHandle *createHandle();
};
//============================================================================

#endif // MINOSSPLITTER_H
