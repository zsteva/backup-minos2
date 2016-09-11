#ifndef SKYSCANDIALOG_H
#define SKYSCANDIALOG_H

#include <QDialog>

namespace Ui {
class SkyScanDialog;
}

class SkyScanDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SkyScanDialog(QWidget *parent = 0);
    ~SkyScanDialog();

private:
    Ui::SkyScanDialog *ui;
};

#endif // SKYSCANDIALOG_H
