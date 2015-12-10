#ifndef TLOCCALCFORM_H
#define TLOCCALCFORM_H

#include <QDialog>

namespace Ui {
class TLocCalcForm;
}

class TLocCalcForm : public QDialog
{
    Q_OBJECT

public:
    explicit TLocCalcForm(QWidget *parent = 0);
    ~TLocCalcForm();

private:
    Ui::TLocCalcForm *ui;
};

#endif // TLOCCALCFORM_H
