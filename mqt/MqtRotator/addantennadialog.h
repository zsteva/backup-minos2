#ifndef ADDANTENNADIALOG_H
#define ADDANTENNADIALOG_H

#include <QDialog>
#include "rotcontrol.h"

namespace Ui {
class AddAntennaDialog;
}

class AddAntennaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddAntennaDialog(QStringList _availAntennas, RotControl* rot, QWidget *parent = nullptr);
    ~AddAntennaDialog();

    QString getAntennaName();
    QString getRotatorModel();

private slots:
    void editingFinished();
    void rotatorModelSelect(int index);
    void accepted();

private:
    Ui::AddAntennaDialog *ui;
    QStringList availAntennas;
    QString antennaName;
    QString rotatorModel;
};

#endif // ADDANTENNADIALOG_H
