#ifndef ADDRADIODIALOG_H
#define ADDRADIODIALOG_H

#include <QDialog>
#include "rigcontrol.h"

namespace Ui {
class AddRadioDialog;
}

class AddRadioDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddRadioDialog(QStringList _availRadios, RigControl* rig, QWidget *parent = nullptr);
    ~AddRadioDialog();

    QString getRadioName();
    QString getRadioModel();


private slots:
    void editingFinished();
    void radioModelSelect(int index);

    void accepted();
private:
    Ui::AddRadioDialog *ui;
    QStringList availRadios;
    QString radioName;
    QString radioModel;


};

#endif // ADDRADIODIALOG_H
