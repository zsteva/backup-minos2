#ifndef TCONTACTLISTDETAILS_H
#define TCONTACTLISTDETAILS_H

#include <QDialog>

namespace Ui {
class TContactListDetails;
}

class TContactListDetails : public QDialog
{
    Q_OBJECT

public:
    explicit TContactListDetails(QWidget *parent = 0);
    ~TContactListDetails();

private:
    Ui::TContactListDetails *ui;
};

#endif // TCONTACTLISTDETAILS_H
