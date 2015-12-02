#ifndef TENTRYOPTIONSFORM_H
#define TENTRYOPTIONSFORM_H

#include "logger_pch.h"

class LoggerContestLog;
enum ExportType;

namespace Ui {
class TEntryOptionsForm;
}

class TOptionFrame;
class TEntryOptionsForm : public QDialog
{
    Q_OBJECT

public:
    explicit TEntryOptionsForm( QWidget* Owner, LoggerContestLog * , bool minosSave );
    ~TEntryOptionsForm();

    QString doFileSave( );
private:
    Ui::TEntryOptionsForm *ui;

    LoggerContestLog * ct;

    std::vector<TOptionFrame *> options;

    ExportType expformat;
    bool minosSave;
    void getContestOperators();
    int opsQSOLine1;
    int opsQSOLine2;
    int opsEntryLine1;
    int opsEntryLine2;

};

#endif // TENTRYOPTIONSFORM_H
