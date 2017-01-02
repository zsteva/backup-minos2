#ifndef TCLOCKDLG_H
#define TCLOCKDLG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class TClockDlg;
}

class TClockDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TClockDlg(QWidget *parent = 0);
    ~TClockDlg();

private slots:
    void on_OKButton_clicked();

    void on_clearButton_clicked();

    void on_applyButton_clicked();

    void on_cancelButton_clicked();

    void on_clockTicked();

    void on_dateEdit_dateChanged(const QDate &date);

    void on_timeEdit_timeChanged(const QTime &time);

private:
    Ui::TClockDlg *ui;
    void setEdits();
    void handleEdit();
    void doCloseEvent();

    bool initialised;
    int initialCorrection;
    QTimer clockTick;
public Q_SLOTS:
    virtual void accept() override;
    virtual void reject() override;

};

#endif // TCLOCKDLG_H
