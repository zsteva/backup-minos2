#ifndef MATCHCOLLECTION_H
#define MATCHCOLLECTION_H
#include "XMPP_pch.h"
#include "contacts.h"
#include "MatchContact.h"

class matchElement
{
   public:
      bool match;
      bool empty;
      //      bool trimmed;

      QString mstr;
      QString rawstr;

      matchElement( void );
      unsigned char set
         ( const QString & );
      unsigned char checkGreater( const QString & );
      bool checkMatch( const QString & );
};

class TMatchCollection
{
   public:
      ContestMatchList contestMatchList;
      int contactCount();
      TMatchCollection( void );
      ~TMatchCollection();
      int getContestCount( void );
      QSharedPointer<BaseMatchContest> pcontestAt( int );
};

typedef QSharedPointer< TMatchCollection > SharedMatchCollection;
Q_DECLARE_METATYPE(SharedMatchCollection)

#endif // MATCHCOLLECTION_H
