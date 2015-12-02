/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////

#ifndef listH
#define listH 
//---------------------------------------------------------------------------
/*
struct LtListSeq
{
   bool operator() ( const ListContact* s1, const ListContact* s2 ) const;
};
typedef codeproject::sorted_vector < ListContact *, true, LtListSeq > ListList;
*/
class ListContact;
typedef std::vector < ListContact *> ListList;
typedef ListList::iterator ListIterator;

class ContactList
{
   private:
      bool cslFile;
      int slotno;
      bool errMessShown;
      bool cslLoad();
      bool cslLoadContacts();
      void freeAll();
   public:
      QString cfileName;
      QString name;

      ListList ctList;
      ContactList();
      ~ContactList();
      bool initialise( int slotno );
      bool initialise( const QString &, int slotno );
      void getMatchText( ListContact *, QString &, const BaseContestLog *const ct ) const;
      void getMatchField( ListContact *pct, int col, QString &disp, const BaseContestLog *const ct ) const;
      int getContactCount( void )
      {
         return ctList.size();
      }
      ListContact *pcontactAt( unsigned int offset );
};
#endif
