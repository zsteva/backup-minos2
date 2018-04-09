#include "rotpresetbutton.h"







RotPresetButton::RotPresetButton(QToolButton *b, int num)
{
    presetNum = num;

    presetButton = b;
    presetMenu = new QMenu(presetButton);

    presetButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    presetButton->setPopupMode(QToolButton::MenuButtonPopup);
    presetButton->setFocusPolicy(Qt::NoFocus);
    presetButton->setText(QString("%1:").arg(QString::number(presetNum + 1)));

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

void RotPresetButton::presetShortCutSelected()
{
//    rigControlFrame->memoryShortCutSelected(memNo);
    presetButton->showMenu();
    //emit lostFocus();
}
void RotPresetButton::readActionSelected()
{
    emit presetReadAction();
}
void RotPresetButton::editActionSelected()
{
    emit presetEditAction();
}
void RotPresetButton::writeActionSelected()
{
    emit presetWriteAction();
}
void RotPresetButton::clearActionSelected()
{
    emit presetClearAction();
}


void RotPresetButton::setText(QString t)
{
    presetButton->setText(t);
}

QString RotPresetButton::getText()
{
    return presetButton->text();
}
