#ifndef RIGMEMORYFRAME_H
#define RIGMEMORYFRAME_H

#include <QFrame>
#include <QShortcut>
#include <QToolButton>
#include <QMenu>

class BaseContestLog;
class LoggerContestLog;

#include "rigmemcommondata.h"

namespace Ui {
class RigMemoryFrame;
}
class RigMemoryFrame;
class RigMemoryButton : public QObject
{
    Q_OBJECT

public:
    explicit RigMemoryButton(QWidget *parent, RigMemoryFrame *rcf, int no);
    ~RigMemoryButton();

    RigMemoryFrame *rigMemoryFrame;
    QToolButton* memButton;
    QMenu* memoryMenu;
    QShortcut* shortKey;
    QShortcut* shiftShortKey;
    QAction* readAction;
    QAction* writeAction;
    QAction* editAction;
    QAction* clearAction;

    int memNo;

private slots:
//    void memoryUpdate();

    void memoryShortCutSelected();
    void readActionSelected();
    void editActionSelected();
    void writeActionSelected();
    void clearActionSelected();
signals:
    void clearActionSelected(int);

};

class RigMemoryFrame : public QFrame
{
    Q_OBJECT

    void sendUpdateMemories();

public:
    explicit RigMemoryFrame(QWidget *parent = 0);
    ~RigMemoryFrame();

//    void reInitialiseMemories();
    void setContest( BaseContestLog *ct );

    void doMemoryUpdates();
    //void memoryUpdate(int);

    void readActionSelected(int);
    void editActionSelected(int buttonNumber);
    void writeActionSelected(int);

private slots:
    void on_newMemoryButton_clicked();

    void on_flushMemoriesButton_clicked();

public slots:
    void clearActionSelected(int);


private:
    Ui::RigMemoryFrame *ui;
    LoggerContestLog *ct;
    // memory buttons

    memoryData::memData getRigMemoryData(int memoryNumber);
    void setRigMemoryData(int memoryNumber, memoryData::memData m);

    QMap<int, QSharedPointer<RigMemoryButton> > memButtonMap;

    void traceMsg(QString msg);

};

#endif // RIGMEMORYFRAME_H
