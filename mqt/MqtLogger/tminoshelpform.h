#ifndef TMINOSHELPFORM_H
#define TMINOSHELPFORM_H

#include <QDialog>

namespace Ui {
class TMinosHelpForm;
}

class TMinosHelpForm : public QDialog
{
    Q_OBJECT

public:
    explicit TMinosHelpForm(QWidget *parent = 0);
    ~TMinosHelpForm();
    void setText(const QString &text);

private:
    Ui::TMinosHelpForm *ui;
};

#endif // TMINOSHELPFORM_H
