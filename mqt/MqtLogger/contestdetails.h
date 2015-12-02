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
};

#endif // CONTESTDETAILS_H
