#ifndef TMINOSHELPFORM_H
#define TMINOSHELPFORM_H

#include "base_pch.h"

namespace Ui {
class TMinosHelpForm;
}

class TMinosHelpForm : public QDialog
{
    Q_OBJECT

public:
    explicit TMinosHelpForm(QWidget *parent = nullptr);
    ~TMinosHelpForm();
    void setText(const QString &text);

private slots:
    void on_CloseButton_clicked();

private:
    Ui::TMinosHelpForm *ui;
};

#endif // TMINOSHELPFORM_H
