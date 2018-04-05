#include "rotpresetbutton.h"




RotPresetButton::RotPresetButton(QToolButton *b, RotControlFrame *rcf, int num)
{
    presetNo = num;
    rotControlFrame = rcf;
    presetButton = b;

    initButton();

}


RotPresetButton::RotPresetButton(QToolButton *b, RotatorMainWindow *rmw, int num)
{
    presetNo = num;
    rotMainWindow = rmw;
    presetButton = b;

    initButton();

}





RotPresetButton::initButton()
{

    presetMenu = new QMenu(presetButton);

    presetButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    presetButton->setPopupMode(QToolButton::MenuButtonPopup);
    presetButton->setFocusPolicy(Qt::NoFocus);
    presetButton->setText(QString::number(presetNo + 1));

    //shortKey = new QShortcut(QKeySequence(runButShortCut[memNo]), memButton);
    //shiftShortKey = new QShortcut(QKeySequence(runButShiftShortCut[memNo]), memButton);
    readAction = new QAction("&Read", presetButton);
    writeAction = new QAction("&Write",presetButton);
    editAction = new QAction("&Edit", presetButton);
    clearAction = new QAction("&Clear",presetButton);
    presetMenu->addAction(readAction);
    presetMenu->addAction(writeAction);
    presetMenu->addAction(editAction);
    presetMenu->addAction(clearAction);
    presetButton->setMenu(presetMenu);

    //connect(shortKey, SIGNAL(activated()), this, SLOT(readActionSelected()));
    //connect(shiftShortKey, SIGNAL(activated()), this, SLOT(memoryShortCutSelected()));
    connect(presetButton, SIGNAL(clicked(bool)), this, SLOT(readActionSelected()));
    connect( readAction, SIGNAL( triggered() ), this, SLOT(readActionSelected()));
    connect( writeAction, SIGNAL( triggered() ), this, SLOT(writeActionSelected()));
    connect( editAction, SIGNAL( triggered() ), this, SLOT(editActionSelected()));
    connect( clearAction, SIGNAL( triggered() ), this, SLOT(clearActionSelected()));
}


RotPresetButton::~RotPresetButton()
{
//    delete memButton;
}
void RotPresetButton::presetUpdate()
{
    if (rotControlFrame)
    {
       rotControlFrame->presetButtonUpdate(presetNo);
    }
    else
    {
        rotMainWindow->presetButtonUpdate(presetNo);
    }

}

void RotPresetButton::presetShortCutSelected()
{
//    rigControlFrame->memoryShortCutSelected(memNo);
    presetButton->showMenu();
    //emit lostFocus();
}
void RotPresetButton::readActionSelected()
{
    if (rotControlFrame)
    {
        rotControlFrame->presetButReadActSel(presetNo);
    }
    else
    {
        rotMainWindow->presetButReadActSel(presetNo);
    }
}
void RotPresetButton::editActionSelected()
{
    if (rotControlFrame)
    {
        rotControlFrame->presetButEditActSel(presetNo);
    }
    else
    {
        rotMainWindow->presetButEditActSel(presetNo);
    }

}
void RotPresetButton::writeActionSelected()
{
    if (rotControlFrame)
    {
        rotControlFrame->presetButWriteActSel(presetNo);
    }
    else
    {
        rotMainWindow->presetButWriteActSel(presetNo);
    }

}
void RotPresetButton::clearActionSelected()
{
    emit clearActionSelected(presetNo);
}

