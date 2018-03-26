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
    explicit AddTransVerterDialog(QVector<BandDetail*> _bands, QStringList &transVertNames, QWidget *parent = 0);
    ~AddTransVerterDialog();

    QString getTransVerterName();



private:
    Ui::AddTransVerterDialog *ui;

    QVector<BandDetail*> bands;

    QString bName;
    QStringList transVerterNames;

    void loadBandSel();
    bool checkBandUsed(const QString &bName, const QStringList &transVertNames);
};

#endif // ADDTRANSVERTERDIALOG_H
