/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2006 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#include "base_pch.h"

MinosLoggerEvents MinosLoggerEvents::mle;
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendContestPageChanged()
{
    emit mle.ContestPageChanged();
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendLogColumnsChanged()
{
    emit mle.LogColumnsChanged();
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendSplittersChanged()
{
    emit mle.SplittersChanged();
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendValidateError(int err)
{
    emit mle.ValidateError(err);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendReportOverstrike(bool ov, BaseContestLog *c)
{
    emit mle.ReportOverstrike(ov, c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendAfterLogContact(BaseContestLog *ct)
{
    emit mle.AfterLogContact(ct);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendAfterSelectContact(QSharedPointer<BaseContact> lct, BaseContestLog *c)
{
    emit mle.AfterSelectContact(lct, c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendNextContactDetailsOnLeft()
{
    emit mle.NextContactDetailsOnLeft();
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendContestDetails(BaseContestLog *c)
{
    emit mle.ContestDetails(c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendGoToSerial(BaseContestLog *c)
{
    emit mle.GoToSerial(c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendMakeEntry(BaseContestLog *c)
{
    emit mle.MakeEntry(c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendNextUnfilled(BaseContestLog *c)
{
    emit mle.NextUnfilled(c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendFormKey(unsigned int *key, BaseContestLog *c)
{
    emit mle.FormKey(key, c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendEditMatchContact(BaseContestLog *c)
{
    emit mle.EditMatchContact(c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendScreenContactChanged(ScreenContact *sct, BaseContestLog *c)
{
    emit mle.ScreenContactChanged(sct, c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendReplaceThisLogList(TMatchCollection *matchCollection, BaseContestLog *c)
{
    emit mle.ReplaceThisLogList(matchCollection, c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendReplaceOtherLogList(TMatchCollection *matchCollection, BaseContestLog *c)
{
    emit mle.ReplaceOtherLogList(matchCollection, c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendReplaceListList(TMatchCollection *matchCollection, BaseContestLog *c)
{
    emit mle.ReplaceListList(matchCollection, c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendScrollToCountry(QString prefix, BaseContestLog *c)
{
    emit mle.ScrollToCountry(prefix, c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendScrollToDistrict(QString prefix, BaseContestLog *c)
{
    emit mle.ScrollToDistrict(prefix, c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendMatchStarting( BaseContestLog *c)
{
    emit mle.MatchStarting(c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendShowOperators()
{
    emit mle.ShowOperators();
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendXferPressed()
{
    emit mle.XferPressed();
}

//---------------------------------------------------------------------------
void MinosLoggerEvents::SendBandMapPressed()
{
    emit mle.BandMapPressed();
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendTimerDistribution()
{
    emit mle.TimerDistribution();
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendFiltersChanged()
{
    emit mle.FiltersChanged();
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendAfterTabFocusIn(QLineEdit *tle)
{
    emit mle.AfterTabFocusIn(tle);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendValidated()
{
    emit mle.Validated();
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendCountrySelect(QString cty, BaseContestLog *c)
{
    emit mle.CountrySelect(cty, c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendDistrictSelect(QString dist, BaseContestLog *c)
{
    emit mle.DistrictSelect(dist, c);
}
//---------------------------------------------------------------------------
void MinosLoggerEvents::SendLocSelect(QString loc, BaseContestLog *c)
{
    emit mle.LocSelect(loc, c);
}
//---------------------------------------------------------------------------

