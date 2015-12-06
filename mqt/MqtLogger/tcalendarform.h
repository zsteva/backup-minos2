#ifndef TCALENDARFORM_H
#define TCALENDARFORM_H

#include <QDialog>

namespace Ui {
class TCalendarForm;
}

class TCalendarForm : public QDialog
{
    Q_OBJECT

public:
    explicit TCalendarForm(QWidget *parent = 0);
    ~TCalendarForm();

private:
    Ui::TCalendarForm *ui;
};

#endif // TCALENDARFORM_H
