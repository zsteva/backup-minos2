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
    explicit TransVertSetupForm(TransVertParams *transvertData, QWidget *parent = nullptr);



        bool transVertValueChanged;
        bool transVertNameChanged;

        void setBand(QString b);
        QString getBand();

        void antSwNumVisible(bool visible);
        QString getTransVerSwNum();
        void setTransVerSwNum(QString s);

        bool getEnableTransVertSw();
        void setEnableTransVertSw(bool b);

        bool getNegCheckBox();
        void setNegCheckBox(bool b);


        void setUiItemsVisible(bool visible);

         void setEnableTransVertSwBoxVisible(bool visible);

         void loadBandFreqLimits();
         void setRadioFreqBox(QString f);
         void setTargetFreqBox(QString f);


         void setOffsetFreqLabel(QString f);



signals:

    public slots:

    private slots:
        //void bandSelected();
        void calcOffset();
        void negCheckBoxSelected(bool);
        void enableTransVertSwSel(bool);
        void transVertSwNumSel();





        void antennaNumSwSel();
private:


        Ui::transVertSetupForm *ui;
        TransVertParams *transVertData;




        void loadBandSel();
    };

    #endif // TRANSVERTSETUPFORM_H
