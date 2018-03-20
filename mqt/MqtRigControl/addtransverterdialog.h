#ifndef ADDTRANSVERTERDIALOG_H
#define ADDTRANSVERTERDIALOG_H

#include <QDialog>
#include "BandList.h"

namespace Ui {
class AddTransVerterDialog;
}

class AddTransVerterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTransVerterDialog(QVector<BandDetail*> _bands, QWidget *parent = 0);
    ~AddTransVerterDialog();

    QString getTransVerterName();
    double getfLow();
    double getfHigh();

private slots:
    void bandSelected(int idx);
private:
    Ui::AddTransVerterDialog *ui;

    QVector<BandDetail*> bands;

    QString bName;
    double fLow = 0.0;
    double fHigh = 0.0;

    void loadBandSel();
};

#endif // ADDTRANSVERTERDIALOG_H
