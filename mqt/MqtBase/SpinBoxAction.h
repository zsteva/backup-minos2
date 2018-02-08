#ifndef SPINBOXACTION_H
#define SPINBOXACTION_H

#include <QWidgetAction>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QLabel>

class SpinBoxAction : public QWidgetAction {
public:
    SpinBoxAction (const QString& title);
    QSpinBox * spinBox ();

private:
    QSpinBox * pSpinBox;
};

#endif // SPINBOXACTION_H
