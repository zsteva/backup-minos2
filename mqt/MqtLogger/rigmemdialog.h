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
    explicit RigMemDialog(QWidget *parent = 0);
    ~RigMemDialog();

    void setDialogTitle(QString number);
    void setLogData(memoryData::memData*, int, QString);
    void setRadioName(QString);
    void setRadioState(QString);

    int readAllMemories();
    memoryData::memData getMemoryData(int memoryNumber);
    void clearMemory(int memoryLoc);
signals:
    void memorySaved(int);


private slots:
    void saveButtonPushed();
    void cancelButtonPushed();
private:
    Ui::RigMemDialog *ui;
    memoryData::memData memoryList[memoryData::NUM_MEMORIES];
    QString radioName;
    QString radioState;
    int memoryNumber;
    memoryData::memData logData;
    int readSettings();
    int saveMemory(int memoryLoc);
    int readMemory(int memoryLoc);


};

#endif // RIGMEMDIALOG_H
