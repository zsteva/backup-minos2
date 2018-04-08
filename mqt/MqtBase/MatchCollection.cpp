#include "base_pch.h"

TMatchCollection::TMatchCollection( void )
{
    qRegisterMetaType< SharedMatchCollection > ( "SharedMatchCollection" );
}
TMatchCollection::~TMatchCollection( void )
{
}
int TMatchCollection::getContestCount( void )
{
   return contestMatchList.size();
}

QSharedPointer<BaseMatchContest> TMatchCollection::pcontestAt( int i )
{
    if (i > contestMatchList.size())
        return QSharedPointer<BaseMatchContest>();

    return std::next(contestMatchList.begin(), i)->wt;
}

int TMatchCollection::contactCount()
{
    int cc = 0;
    for (ContestMatchIterator i = contestMatchList.begin(); i != contestMatchList.end(); i++ )
    {
        cc += i->wt->contactMatchList.size();
    }
    return cc;
}

