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

#include <XMPP_pch.h>
//---------------------------------------------------------------------------
class BaseContestLog;
class BaseContact;
class ScreenContact;
class TMatchCollection;

class MinosLoggerEvents:public QObject
{
    Q_OBJECT

private:
   static MinosLoggerEvents mle;

signals:
   void ContestPageChanged();
   void LogColumnsChanged();
   void SplittersChanged();
   void ValidateError(int err);
   void ShowErrorList();
   void ReportOverstrike(bool ov, BaseContestLog *c);
   void AfterLogContact(BaseContestLog *ct);
   void AfterSelectContact(BaseContact *ct, BaseContestLog *);
   void NextContactDetailsOnLeft();
   void ContestDetails(BaseContestLog *);
   void GoToSerial(BaseContestLog *);
   void MakeEntry(BaseContestLog *);
   void NextUnfilled(BaseContestLog *);
   void FormKey(unsigned int *, BaseContestLog *);
   void SetMode(QString, BaseContestLog *);
   void SetFreq(QString, BaseContestLog *);
   void EditMatchContact(BaseContestLog *);
   void ScreenContactChanged(ScreenContact *, BaseContestLog *);
   void ReplaceLogList(TMatchCollection *matchCollection, BaseContestLog *);
   void ReplaceListList(TMatchCollection *matchCollection, BaseContestLog *);
   void ScrollToCountry(std::string prefix, BaseContestLog *);
   void ScrollToDistrict(std::string prefix, BaseContestLog *);
   void MatchStarting(BaseContestLog *);
   void Op1Change(std::string op1, BaseContestLog *);
   void Op2Change(std::string op2, BaseContestLog *);
   void ShowOperators();

   void CountrySelect(std::string cty, BaseContestLog *c);
   void DistrictSelect(std::string dist, BaseContestLog *c);
   void LocSelect(std::string loc, BaseContestLog *c);
public:
   static void SendContestPageChanged();
   static void SendLogColumnsChanged();
   static void SendSplittersChanged();
   static void SendValidateError(int err);
   static void SendShowErrorList();
   static void SendReportOverstrike(bool ov, BaseContestLog *c);
   static void SendAfterLogContact(BaseContestLog *ct);
   static void SendAfterSelectContact(BaseContact *ct, BaseContestLog *);
   static void SendNextContactDetailsOnLeft();
   static void SendContestDetails(BaseContestLog *);
   static void SendGoToSerial(BaseContestLog *);
   static void SendMakeEntry(BaseContestLog *);
   static void SendNextUnfilled(BaseContestLog *);
   static void SendFormKey(unsigned int *, BaseContestLog *);
   static void SendSetMode(QString, BaseContestLog *);
   static void SendSetFreq(QString, BaseContestLog *);
   static void SendEditMatchContact(BaseContestLog *);
   static void SendScreenContactChanged(ScreenContact *, BaseContestLog *);
   static void SendReplaceLogList(TMatchCollection *matchCollection, BaseContestLog *);
   static void SendReplaceListList(TMatchCollection *matchCollection, BaseContestLog *);
   static void SendScrollToCountry(std::string prefix, BaseContestLog *);
   static void SendScrollToDistrict(std::string prefix, BaseContestLog *);
   static void SendMatchStarting(BaseContestLog *);
   static void SendOp1Change(std::string op1, BaseContestLog *);
   static void SendOp2Change(std::string op2, BaseContestLog *);
   static void SendShowOperators();

   static void SendCountrySelect(std::string cty, BaseContestLog *c);
   static void SendDistrictSelect(std::string dist, BaseContestLog *c);
   static void SendLocSelect(std::string loc, BaseContestLog *c);

};
//---------------------------------------------------------------------------
#endif
