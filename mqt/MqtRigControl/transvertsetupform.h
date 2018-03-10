#ifndef TRANSVERTSETUPFORM_H
#define TRANSVERTSETUPFORM_H

#include <QWidget>
#include "rigcontrol.h"
#include "ui_transvertsetupform.h"
#include "BandList.h"

namespace Ui {
    class transVertSetupForm;
}

class TransVertSetupForm : public QWidget
{
    Q_OBJECT
public:
    explicit TransVertSetupForm(QWidget *parent = nullptr);


    bool tansVertValueChanged;
    bool transVertNameChanged;



    QString getTransVerSwNum();
    void setTransVerSwNum(QString s);

    bool getEnableTransVertSw();
    void setEnableTransVertSw(bool b);

    bool getNegCheckBox();
    void setNegCheckBox(bool b);

    QString getTransVertOffsetFreq();
    void setTransVertOffsetFreq(QString s);

signals:

public slots:

private slots:
    void bandSelected();
    void transVertOffsetSelected();
    void negCheckBoxSelected(bool);
    void enableTransVertSwSel(bool);
    void transVertSwNumSel();


private:


    Ui::transVertSetupForm *ui;




    void loadBands();
};

#endif // TRANSVERTSETUPFORM_H
