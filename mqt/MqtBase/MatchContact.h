/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#ifndef MatchContactH
#define MatchContactH 
//---------------------------------------------------------------------------
class ListContact;
class ContactList;
class BaseMatchContest;

class LtMatchContest
{
public:
   bool operator() ( const BaseMatchContest* s1, const BaseMatchContest* s2 ) const;
};

class LtMatch
{
public:
   bool operator() ( const MatchContact* s1, const MatchContact* s2 ) const;
};
typedef codeproject::sorted_vector < BaseMatchContest *, true, LtMatchContest > ContestMatchList;
typedef ContestMatchList::iterator ContestMatchIterator;

typedef codeproject::sorted_vector < MatchContact *, true, LtMatch > MatchList;
typedef MatchList::iterator MatchIterator;


class MatchContact
{
   public:
      MatchContact( );
      virtual ~MatchContact();
      virtual ContactList *getContactList() const
      {
         return 0;
      }
      virtual ListContact *getListContact() const
      {
         return 0;
      }
      virtual BaseContestLog *getContactLog() const
      {
         return 0;
      }
      virtual BaseContact *getBaseContact() const
      {
         return 0;
      }
      virtual bool operator<( const MatchContact& rhs ) const = 0;
      virtual bool operator==( const MatchContact& rhs ) const = 0;
      virtual bool operator!=( const MatchContact& rhs ) const = 0;
      virtual void getText( QString &dest, BaseContestLog *const ct ) const = 0;
};
class BaseMatchContest
{
public:
   virtual ~BaseMatchContest(){}

   BaseLogList *matchedContest;

   MatchList matchList;

   void freeAll()
   {
      for (MatchIterator i = matchList.begin(); i != matchList.end(); i++ )
      {
         delete ( *i );
         ( *i ) = 0;
      }
      matchList.clear();
   }

   virtual int getContactCount(){return matchList.size();}
   MatchContact *pcontactAt( int );

   virtual ContactList *getContactList() const
   {
      return 0;
   }
   virtual BaseContestLog *getContactLog() const
   {
      return 0;
   }
   virtual bool operator<( const BaseMatchContest& rhs ) const = 0;
};
class MatchContactList : public BaseMatchContest
{
public:
   virtual ContactList *getContactList() const override
   {
      return dynamic_cast<ContactList *>(matchedContest);
   }
   virtual bool operator<( const BaseMatchContest& rhs ) const override;
};
class MatchContactLog : public BaseMatchContest
{
public:
   virtual BaseContestLog *getContactLog() const override
   {
      return dynamic_cast<BaseContestLog *>(matchedContest);
   }
   virtual bool operator<( const BaseMatchContest& rhs ) const override;
};


class MatchListContact: public MatchContact
{
   public:
      ContactList *matchedList;
      ListContact *matchedListContact;
      MatchListContact( ContactList *ct, ListContact *lc );
      virtual ~MatchListContact();
      virtual ContactList *getContactList() const
      {
         return matchedList;
      }
      virtual ListContact *getListContact() const
      {
         return matchedListContact;
      }
      virtual bool operator<( const MatchContact& rhs ) const;
      virtual bool operator==( const MatchContact& rhs ) const;
      virtual bool operator!=( const MatchContact& rhs ) const;
      virtual void getText(QString &dest, BaseContestLog *const ct ) const override;
};
class MatchLogContact: public MatchContact
{
   public:
      BaseContestLog *matchedContest;
      BaseContact *matchedContact;
      MatchLogContact( BaseContestLog *ct, BaseContact *lc );
      virtual ~MatchLogContact();
      virtual BaseContestLog *getContactLog() const
      {
         return matchedContest;
      }
      virtual BaseContact *getBaseContact() const
      {
         return matchedContact;
      }
      virtual bool operator<( const MatchContact& rhs ) const;
      virtual bool operator==( const MatchContact& rhs ) const;
      virtual bool operator!=( const MatchContact& rhs ) const;
      virtual void getText( QString &dest, BaseContestLog *const ct ) const override;
};
#endif
