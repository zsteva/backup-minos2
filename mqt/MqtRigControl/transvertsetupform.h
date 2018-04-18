#ifndef TRANSVERTSETUPFORM_H
#define TRANSVERTSETUPFORM_H

#include <QWidget>
#include "rigcontrol.h"
#include "ui_transvertsetupform.h"
#include "BandList.h"
#include "focuswatcher.h"


namespace Ui {
    class transVertSetupForm;
}




class TransVertSetupForm : public QWidget
{
    Q_OBJECT
public:
    explicit TransVertSetupForm(TransVertParams* _transvertData, QWidget *parent = nullptr);



        bool transVertValueChanged = false;
        bool transVertNameChanged = false;

        bool transVertOffsetOk = false;

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



         void setEnableLocalTransVertSwVisible(bool visible);
         bool getEnableLocalTransVertSw();
         void setEnableLocalTransVertSw(bool b);


         QString getLocTVSwComport();
         void setLocTVSwComport(QString p);
         void setLocTVSWComportVisible(bool visible);


signals:

    public slots:

    private slots:
        //void bandSelected();
        void calcOffset();
        void enableTransVertSwSel(bool);
        void transVertSwNumSel();

        void localTransVertSwSel(bool);
        void locTVComPortSel(int);


private:


        Ui::transVertSetupForm *ui;
        TransVertParams *transVertData;

        FocusWatcher *radioFreqEdit;
        FocusWatcher *targetFreqEdit;

        bool radioFreqOK = false;
        bool targetFreqOK = false;



        void loadBandSel();
public Q_SLOTS:
    void radioFreqEditfocusChange(QObject *, bool, QFocusEvent *event);
    void targetFreqEditfocusChange(QObject *, bool, QFocusEvent *event);
};

    #endif // TRANSVERTSETUPFORM_H
