#ifndef CONTESTDETAILS_H
#define CONTESTDETAILS_H

#include <QDialog>
#include "tbundleframe.h"

namespace Ui {
class ContestDetails;
}

class LoggerContestLog;
class IndividualContest;

class ContestDetails : public QDialog
{
    Q_OBJECT

public:
    explicit ContestDetails(QWidget *parent = 0);
    ~ContestDetails();

private:
    Ui::ContestDetails *ui;

private:   	// User declarations
//      TCalendarForm *CalendarDlg;
   LoggerContestLog * contest;
   LoggerContestLog * inputcontest;
   QString sectionList;
   bool saveContestOK;
   bool suppressProtectedOnClick;

   QWidget * getNextFocus();
   void setDetails( const IndividualContest &ic );
   void setDetails( );
   void refreshOps();
   void enableControls();
   QWidget * getDetails( );
public:   		// User declarations
   void setDetails( LoggerContestLog * ct );
   void bundleChanged();

   virtual int exec() override;
private slots:
   void on_OKButton_clicked();
   void on_EntDetailButton_clicked();
   void on_CancelButton_clicked();
   void on_BSHelpButton_clicked();
   void on_VHFCalendarButton_clicked();
   void on_CallsignEdit_editingFinished();
   void on_DXCCMult_clicked();
   void on_NonGCtryMult_clicked();
   void on_LocatorMult_clicked();
   void on_GLocMult_clicked();
   void on_M7LocatorMults_clicked();
   void on_ProtectedOption_clicked();
};

#endif // CONTESTDETAILS_H
