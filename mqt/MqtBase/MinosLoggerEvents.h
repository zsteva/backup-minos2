/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2006 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#ifndef MinosLoggerEventsH
#define MinosLoggerEventsH

#include "XMPP_pch.h"
//---------------------------------------------------------------------------
class BaseContestLog;
class BaseContact;
class ScreenContact;
class TMatchCollection;
class QLineEdit;

class MinosLoggerEvents:public QObject
{
    Q_OBJECT

signals:
   void ContestPageChanged();
   void LogColumnsChanged();
   void SplittersChanged();
   void ValidateError(int err);
   void ReportOverstrike(bool ov, BaseContestLog *c);
   void AfterLogContact(BaseContestLog *ct);
   void AfterSelectContact(QSharedPointer<BaseContact> ct, BaseContestLog *);
   void NextContactDetailsOnLeft();
   void ContestDetails(BaseContestLog *);
   void GoToSerial(BaseContestLog *);
   void MakeEntry(BaseContestLog *);
   void NextUnfilled(BaseContestLog *);
   void FormKey(unsigned int *, BaseContestLog *);
   void ScreenContactChanged(ScreenContact *, BaseContestLog *);
   void ReplaceThisLogList(TMatchCollection *matchCollection, BaseContestLog *);
   void ReplaceOtherLogList(TMatchCollection *matchCollection, BaseContestLog *);
   void ReplaceListList(TMatchCollection *matchCollection, BaseContestLog *);
   void ScrollToCountry(QString prefix, BaseContestLog *);
   void ScrollToDistrict(QString prefix, BaseContestLog *);
   void MatchStarting(BaseContestLog *);
   void ShowOperators();
   void BandMapPressed();
   void TimerDistribution();
   void FiltersChanged();
   void AfterTabFocusIn(QLineEdit *tle);
   void Validated();

   void CountrySelect(QString cty, BaseContestLog *c);
   void DistrictSelect(QString dist, BaseContestLog *c);
   void LocSelect(QString loc, BaseContestLog *c);

   void FontChanged();
public:
   static MinosLoggerEvents mle;

   static void SendContestPageChanged();
   static void SendLogColumnsChanged();
   static void SendSplittersChanged();
   static void SendValidateError(int err);
   static void SendReportOverstrike(bool ov, BaseContestLog *c);
   static void SendAfterLogContact(BaseContestLog *ct);
   static void SendAfterSelectContact(QSharedPointer<BaseContact> ct, BaseContestLog *);
   static void SendNextContactDetailsOnLeft();
   static void SendContestDetails(BaseContestLog *);
   static void SendGoToSerial(BaseContestLog *);
   static void SendMakeEntry(BaseContestLog *);
   static void SendNextUnfilled(BaseContestLog *);
   static void SendFormKey(unsigned int *, BaseContestLog *);

   static void SendScreenContactChanged(ScreenContact *, BaseContestLog *);
   static void SendReplaceThisLogList(TMatchCollection *matchCollection, BaseContestLog *);
   static void SendReplaceOtherLogList(TMatchCollection *matchCollection, BaseContestLog *);
   static void SendReplaceListList(TMatchCollection *matchCollection, BaseContestLog *);
   static void SendScrollToCountry(QString prefix, BaseContestLog *);
   static void SendScrollToDistrict(QString prefix, BaseContestLog *);
   static void SendMatchStarting(BaseContestLog *);
   static void SendShowOperators();
   static void SendBandMapPressed();
   static void SendTimerDistribution();
   static void SendFiltersChanged();
   static void SendAfterTabFocusIn(QLineEdit *tle);
   static void SendValidated();

   static void SendCountrySelect(QString cty, BaseContestLog *c);
   static void SendDistrictSelect(QString dist, BaseContestLog *c);
   static void SendLocSelect(QString loc, BaseContestLog *c);

   static void SendFontChanged();
};
//---------------------------------------------------------------------------
#endif
