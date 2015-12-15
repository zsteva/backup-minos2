#ifndef MINOSSPLITTER_H
#define MINOSSPLITTER_H

#include <QWidget>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLinearGradient>
#include <QSplitter>
#include <QSplitterHandle>
#include <QPainter>
#include <QPaintEvent>
#include <QSizePolicy>

//============================================================================
class MinosSplitterButton : public QPushButton
{
    Q_OBJECT
private:
    Qt::Orientation orientation;
    QStyleOptionButton getStyleOption() const;

protected:
    void paintEvent(QPaintEvent *event);
public:
    MinosSplitterButton(Qt::Orientation orientation,const QString & text, QWidget * parent);
    QSize sizeHint() const;
};

class MinosSplitterHandle : public QSplitterHandle
{
    Q_OBJECT
private:
    QWidget *closingWidget;
    MinosSplitterButton *splitterButton;
    int oldHeight;
    int oldWidth;
public:
    MinosSplitterHandle(Qt::Orientation orientation, QSplitter *parent);
    void setClosingWidget(QWidget *);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);

private slots:
    void splitterButtonPushed();

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
   void setOrientation(Qt::Orientation orientation);

   void setClosingWidget(QWidget *);
protected:
   QSplitterHandle *createHandle();
};
//============================================================================

#endif // MINOSSPLITTER_H
