#ifndef RIGMEMDIALOG_H
#define RIGMEMDIALOG_H

#include <QDialog>
#include <QSettings>
#include "rigmemcommondata.h"

namespace Ui {
class RigMemDialog;
}

class RigMemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RigMemDialog(QString radioName, QString radioState, QWidget *parent = 0);
    ~RigMemDialog();

    void setDialogTitle(QString number);
private:
    Ui::RigMemDialog *ui;
    memoryData::memData memoryList[memoryData::NUM_MEMORIES];
    QString radioName;
    QString radioState;
    int memoryNumber;
    int readSettings();
    int saveMemory(int memoryLoc);
    int readMemory(int memoryLoc);
    int readAllMemories();
    void clearMemory(int memoryLoc);
};

#endif // RIGMEMDIALOG_H
