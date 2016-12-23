/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "base_pch.h"

//---------------------------------------------------------------------------
/*
bool LtMatch::operator() ( const MatchContact* s1, const MatchContact* s2 ) const
{
   return * s1 < *s2;
}
bool LtMatchContest::operator() ( const BaseMatchContest* s1, const BaseMatchContest* s2 ) const
{
   return * s1 < *s2;
}
*/

QSharedPointer<MatchContact> BaseMatchContest::pcontactAt( int i )
{
    if (i > static_cast< int > (matchList.size()))
        return QSharedPointer<MatchContact>();
    return *std::next(matchList.begin(), i);
    // ERROR
    //return 0;
    /*
    return matchList.at( i );
    */
}

bool MatchContactList::operator<( const BaseMatchContest& rhs ) const
{
   // p1 is from list; p2 is the one being searched for
    // p1 is from list; p2 is the one being searched for

    const ContactList * cn1 = getContactList();
    const ContactList * cn2 = rhs.getContactList();

    if ( cn2 == 0 )       // cn1 must be good - its us!
    {
       return false;
    }
    if ( cn1 != cn2 )
    {
       if ( cn1 < cn2 )
          return true;
       else
          return false;
    }

    return false;
}
bool MatchContactLog::operator<( const BaseMatchContest& rhs ) const
{
   // p1 is from list; p2 is the one being searched for

   const BaseContestLog * cn1 = getContactLog();
   const BaseContestLog * cn2 = rhs.getContactLog();

   if ( cn2 == 0 )
   {
      return true;
   }
   if ( cn1 != cn2 )
   {
       if ( cn1 < cn2 )
          return true;
       else
          return false;
   }

   return false;

}

MatchContact::MatchContact( )
{}
MatchContact::~MatchContact()
{}
//==============================================================================
MatchLogContact::MatchLogContact(BaseContestLog *ct, QSharedPointer<BaseContact> lc )
      : matchedContest( ct ), matchedContact( lc )
{}
MatchLogContact::~MatchLogContact()
{}

void MatchLogContact::getText(QString &dest, BaseContestLog *const ct ) const
{
   BaseContestLog * clp = matchedContest;
   clp->getMatchText( matchedContact, dest, ct );
}
bool MatchLogContact::operator<( const MatchContact& rhs ) const
{
   // p1 is from list; p2 is the one being searched for

   BaseContestLog *cn1 = getContactLog();
   BaseContestLog *cn2 = rhs.getContactLog();

   if ( cn2 == 0 )
   {
      return true;
   }
   if ( cn1 != cn2 )
   {
      // as we now separate current contest and others...
      BaseContestLog * cc = MinosParameters::getMinosParameters() ->getCurrentContest();
      if ( cn1 == cc )
         return true;
      else
         if ( cn2 == cc )
            return false;
      if ( cn1->cslotno < cn2->cslotno )
         return true;
      else
         return false;
   }

   unsigned long p1 = getBaseContact() ->getLogSequence();
   unsigned long p2 = rhs.getBaseContact() ->getLogSequence();

   if ( p1 == p2 )
      return false;

   // This one is more problematic
   const BaseContestLog * ct = MinosParameters::getMinosParameters() ->getCurrentContest();
   if ( bool( ct ) && ( ct->DupSheet.isCurDup( getBaseContact() ) ) )
      return true;

   if ( p1 < p2 )
      return true;

   return false;

}
bool MatchLogContact::operator==( const MatchContact& rhs ) const
{
   // p1 is from list; p2 is the one being searched for

   BaseContestLog *cn1 = getContactLog();
   BaseContestLog *cn2 = rhs.getContactLog();

   if ( cn2 == 0 )
   {
      return true;
   }

   if ( cn1 != cn2 )
   {
      return false;
   }

   return ( getBaseContact() == rhs.getBaseContact() );
}
bool MatchLogContact::operator!=( const MatchContact& rhs ) const
{
   // p1 is from list; p2 is the one being searched for

    BaseContestLog *cn1 = getContactLog();
    BaseContestLog *cn2 = rhs.getContactLog();

   if ( cn2 == 0 )
   {
      return true;
   }

   if ( cn1 != cn2 )
   {
      return true;
   }

   return ( getBaseContact() != rhs.getBaseContact() );
}
//==============================================================================
MatchListContact::MatchListContact(ContactList *ct, ListContact *lc )
      : matchedList( ct ), matchedListContact( lc )
{}
MatchListContact::~MatchListContact()
{}

void MatchListContact::getText( QString &dest, BaseContestLog *const ct ) const
{
   ContactList *clp = matchedList;
   clp->getMatchText( matchedListContact, dest, ct );
}
bool MatchListContact::operator<( const MatchContact& rhs ) const
{
   // p1 is from list; p2 is the one being searched for

   ContactList *cn1 = getContactList();
   ContactList *cn2 = rhs.getContactList();

   if ( cn2 == 0 )       // cn1 must be good - its us!
   {
      return false;
   }
   if ( cn1 != cn2 )
   {
      if ( cn1 < cn2 )
         return true;
      else
         return false;
   }

   ListContact *p1 = getListContact();
   ListContact *p2 = rhs.getListContact();

   if ( p1 == p2 )
      return false;

   if ( p1 < p2 )
      return true;

   return false;

}
bool MatchListContact::operator==( const MatchContact& rhs ) const
{
   // p1 is from list; p2 is the one being searched for

   ContactList *cn1 = getContactList();
   ContactList *cn2 = rhs.getContactList();

   if ( cn1 != cn2 )
   {
      return false;
   }

   return ( getListContact() == rhs.getListContact() );
}
bool MatchListContact::operator!=( const MatchContact& rhs ) const
{
   // p1 is from list; p2 is the one being searched for

   ContactList *cn1 = getContactList();
   ContactList *cn2 = rhs.getContactList();

   if ( cn1 != cn2 )
   {
      return true;
   }

   return ( getContactList() != rhs.getContactList() );
}
//==============================================================================

