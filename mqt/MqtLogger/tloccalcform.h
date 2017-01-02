#ifndef TLOCCALCFORM_H
#define TLOCCALCFORM_H

#include <QDialog>
#include <QLineEdit>
#include "focuswatcher.h"

namespace Ui {
class TLocCalcForm;
}

class TLocCalcForm : public QDialog
{
    Q_OBJECT

public:
    explicit TLocCalcForm(QWidget *parent = 0);
    ~TLocCalcForm();
    virtual int exec() override;

    QString S1Loc;
    QString Distance;

private slots:
    void on_CalcButton_clicked();

    void on_ExitButton_clicked();

    void on_CancelButton_clicked();

private:
    Ui::TLocCalcForm *ui;
    bool Modal;
    void doCloseEvent();
    bool handleExit( QLineEdit *Edit );

    FocusWatcher *S1LocFW;
    FocusWatcher *S1LatLongFW;
    FocusWatcher *S1NGRFW;
    FocusWatcher *S2LocFW;
    FocusWatcher *S2LatLongFW;
    FocusWatcher *S2NGRFW;

public Q_SLOTS:
    void focusChange(QObject *, bool, QFocusEvent *event);
    virtual void accept() override;
    virtual void reject() override;
};

#endif // TLOCCALCFORM_H
