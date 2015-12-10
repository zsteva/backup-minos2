#ifndef TQSOEDITDLG_H
#define TQSOEDITDLG_H

#include <QDialog>

namespace Ui {
class TQSOEditDlg;
}

class TQSOEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TQSOEditDlg(QWidget *parent = 0);
    ~TQSOEditDlg();

private:
    Ui::TQSOEditDlg *ui;
};

#endif // TQSOEDITDLG_H
