#ifndef FOCUSWATCHER
#define FOCUSWATCHER

#include <QObject>
#include <QEvent>

class FocusWatcher : public QObject
{
   Q_OBJECT
public:
   explicit FocusWatcher(QObject* parent = nullptr) : QObject(parent)
   {
      if (parent)
         parent->installEventFilter(this);
   }
   virtual bool eventFilter(QObject *obj, QEvent *event) override
   {
      Q_UNUSED(obj)
      if (event->type() == QEvent::FocusIn)
         emit focusChanged(obj, true);
      else if (event->type() == QEvent::FocusOut)
         emit focusChanged(obj, false);

      return false;
   }

Q_SIGNALS:
   void focusChanged(QObject *obj, bool in);
};
#endif // FOCUSWATCHER

