#ifndef STACKEDINFOFRAME_H
#define STACKEDINFOFRAME_H

#include "logger_pch.h"

namespace Ui {
class StackedInfoFrame;
}

class StackedInfoFrame : public QFrame
{
    Q_OBJECT

public:
    explicit StackedInfoFrame(QWidget *parent = 0);
    ~StackedInfoFrame();

    void setContest(BaseContestLog *contest);
    void refreshMults();
    void getSplitters();

private:
    Ui::StackedInfoFrame *ui;

    int splitterHandleWidth;
    BaseContestLog *contest;
    bool filterClickEnabled;

    void initFilters();
    void saveFilters();
private slots:
    void on_ScrollToDistrict( const QString &qth, BaseContestLog* );
    void on_ScrollToCountry( const QString &csCs, BaseContestLog* );

    void on_WorkedCB_clicked();
    void on_UnworkedCB_clicked();
    void on_ContEU_clicked();
    void on_ContOC_clicked();
    void on_ContAS_clicked();
    void on_ContSA_clicked();
    void on_ContAF_clicked();
    void on_ContNA_clicked();

    void on_StackedMults_currentChanged(int arg1);
    void on_LocatorSplitterMoved(int /*pos*/, int /*index*/);
    void onFiltersChanged();
};

#endif // STACKEDINFOFRAME_H
