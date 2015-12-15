#include "minossplitter.h"
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QMenu>
//============================================================================
// NB - throughout orientation refers to the splitter layout:
// vertical is two areas above and below
// horizontal is two areas one each side
// button code influenced by (see below)
// http://www.qtcentre.org/wiki/index.php?title=OrientationButton
//============================================================================
void MinosSplitterButton::paintEvent(QPaintEvent */*event*/)
{
    QStylePainter p(this);
    p.save();               // save it so we don't keep rotating it
    if (orientation == Qt::Horizontal )
    {
        // we have to move the pivot point as the button is clipped
        p.rotate(90.0);
        p.translate(0, -width());    // dx, dy
    }
    QStyleOptionButton option = getStyleOption();
    //initStyleOption(&option);
    p.drawControl(QStyle::CE_PushButton, option);

    p.restore();
}
QSize MinosSplitterButton::sizeHint() const
{
    QSize size = QPushButton::sizeHint();
    if (orientation == Qt::Horizontal)
        size.transpose();
    return size;
}
QStyleOptionButton MinosSplitterButton::getStyleOption() const
{
    QStyleOptionButton opt;

    opt.initFrom(this);
    if (orientation == Qt::Horizontal)
    {
        QSize size = opt.rect.size();
        size.transpose();
        opt.rect.setSize(size);
    }
    opt.features = QStyleOptionButton::None;
    if (isFlat())
        opt.features |= QStyleOptionButton::Flat;
    if (menu())
        opt.features |= QStyleOptionButton::HasMenu;
    if (autoDefault() || isDefault())
        opt.features |= QStyleOptionButton::AutoDefaultButton;
    if (isDefault())
        opt.features |= QStyleOptionButton::DefaultButton;
    if (isDown() || (menu() && menu()->isVisible()))
        opt.state |= QStyle::State_Sunken;
    if (isChecked())
        opt.state |= QStyle::State_On;
    if (!isFlat() && !isDown())
        opt.state |= QStyle::State_Raised;
    opt.text = text();
    opt.icon = icon();
    opt.iconSize = iconSize();
    return opt;
}

MinosSplitterButton::MinosSplitterButton(Qt::Orientation orientation, const QString & text, QWidget * parent)
    : QPushButton(text, parent), orientation(orientation)
{
    // how do we get a horizontal splitter button to be narrow and tall?
    switch (orientation)
     {
     case Qt::Horizontal:
        // horizontal, vertical
         setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
         break;

     case Qt::Vertical:
        // OK
         setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
         break;
     }
}

//============================================================================
MinosSplitterHandle::MinosSplitterHandle(Qt::Orientation orientation, QSplitter *parent)
    : QSplitterHandle(orientation, parent), oldHeight(-1), oldWidth(-1), closingWidget(0)
{
    gradient.setColorAt(0.0, Qt::darkGreen);
    gradient.setColorAt(0.25, Qt::white);
    gradient.setColorAt(1.0, Qt::darkGreen);

    // We need to also embed a pretty (oriented) button that
    // shows open/closed on the splitter as well
    // This button is pushable, and  nicer than double
    // clicking on the splitter

    // Ideally the text would be an arrow pointing to the "close" direction
    splitterButton = new MinosSplitterButton(orientation, "||...||", this);
    splitterButton->setStatusTip("Open/Close Splitter");

    connect(splitterButton, SIGNAL(clicked()),  this, SLOT(splitterButtonPushed()));

    // How do I set the colour of a button?
    // And I haven't yet been able to get it to show...

    // from   http://www.qtcentre.org/threads/36317-changing-button-background-color
    QPalette p=palette();
    p.setBrush(QPalette::Button,Qt::red);
    p.setBrush(QPalette::ButtonText, Qt::blue);
    splitterButton->setPalette(p);

    QBoxLayout *l;

    if (orientation == Qt::Horizontal)
    {
        l = new QVBoxLayout();
    }
    else
    {
        l = new QHBoxLayout();
    }

    l->setSpacing(0);
    l->setContentsMargins(0, 0, 0, 0);       // THIS is what makes the button full height in the splitter!
    l->addStretch();

    l->addWidget(splitterButton);

    l->addStretch();
    setLayout(l);
}
void MinosSplitterHandle::setClosingWidget(QWidget *w)
{
   closingWidget = w;
   oldHeight = w->maximumHeight();
   oldWidth = w->maximumWidth();
}
void MinosSplitterHandle::splitterButtonPushed()
{
    mouseDoubleClickEvent(0);
}

void MinosSplitterHandle::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (orientation() == Qt::Horizontal) {
        gradient.setStart(rect().left(), rect().height()/2);
        gradient.setFinalStop(rect().right(), rect().height()/2);
        painter.fillRect(event->rect(), QBrush(gradient));

    } else {
        gradient.setStart(rect().width()/2, rect().top());
        gradient.setFinalStop(rect().width()/2, rect().bottom());
        painter.fillRect(event->rect(), QBrush(gradient));
    }
}


void MinosSplitterHandle::mouseMoveEvent(QMouseEvent *e)
{
   // Can we change the colour while over the widget?
   QSplitterHandle::mouseMoveEvent(e);
}

void MinosSplitterHandle::mousePressEvent(QMouseEvent *e)
{
   QSplitterHandle::mousePressEvent(e);
}

void MinosSplitterHandle::mouseReleaseEvent(QMouseEvent *e)
{
   QSplitterHandle::mouseReleaseEvent(e);
}
void MinosSplitterHandle::mouseDoubleClickEvent(QMouseEvent *e)
{
   // we actually want this to open/close the splitter
   // how does this work with a splitter with multiple widgets?
   // A splitter will have multiple handles?

   // Can I find the associated widgets in any way?
   // Actually I think we need to tell the handle
   // who to close

   if (closingWidget && orientation() == Qt::Horizontal)
   {
       if (closingWidget->maximumWidth() == 0)
       {
          closingWidget->setMaximumWidth(oldWidth);
       }
       else
       {
          closingWidget->setMaximumWidth(0);
       }
   }
   else if (closingWidget)
   {
      if (closingWidget->maximumHeight() == 0)
      {
         closingWidget->setMaximumHeight(oldHeight);
      }
      else
      {
         closingWidget->setMaximumHeight(0);
      }
   }
   if (e)
   {
       // we might have called ourselves with a null event
      e->accept();
   }
}

//============================================================================
MinosSplitter::MinosSplitter(QWidget *parent)
    : QSplitter(parent), myHandle(0)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    setHandleWidth(10);
}
void MinosSplitter::setOrientation(Qt::Orientation orientation)
{
    QSplitter::setOrientation(orientation);

}

QSplitterHandle *MinosSplitter::createHandle()
{
    myHandle = new MinosSplitterHandle(orientation(), this);
    return myHandle;
}
void MinosSplitter::setClosingWidget(QWidget *w)
{
   // We really should check that this is one of our widgets...
   if (myHandle)
   {
      myHandle->setClosingWidget(w);
   }
}
