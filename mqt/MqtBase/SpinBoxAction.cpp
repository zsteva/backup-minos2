#include "SpinBoxAction.h"

SpinBoxAction::SpinBoxAction (const QString& title) :
      QWidgetAction (NULL)
{
    QWidget* pWidget = new QWidget (NULL);
    QHBoxLayout* pLayout = new QHBoxLayout();
    QLabel *pLabel = new QLabel (title);
    pLayout->addWidget (pLabel);
    pSpinBox = new QSpinBox(NULL);
    pLayout->addWidget (pSpinBox);
    pWidget->setLayout (pLayout);

    setDefaultWidget(pWidget);
}

QSpinBox * SpinBoxAction::spinBox ()
{
    return pSpinBox;
}

