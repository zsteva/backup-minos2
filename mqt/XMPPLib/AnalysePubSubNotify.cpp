#include "AnalysePubSubNotify.h"
#include "ConfigFile.h"

AnalysePubSubNotify::AnalysePubSubNotify(bool err, QSharedPointer<MinosRPCObj> mro ) :
      OK( false )
{
   if ( !err )
   {
      QSharedPointer<RPCParam> psPublisher;
      QSharedPointer<RPCParam> psServer;
      QSharedPointer<RPCParam> psCategory;
      QSharedPointer<RPCParam> psKey;
      QSharedPointer<RPCParam> psValue;
      QSharedPointer<RPCParam> psState;
      RPCArgs *args = mro->getCallArgs();
      if (
              // NB publisher is initiated by the "from" from the publisher
              // and persists with the publihe entity
         args->getStructArgMember( 0, "Publisher", psPublisher )
         && args->getStructArgMember( 0, "Server", psServer )
         && args->getStructArgMember( 0, "Category", psCategory )
         && args->getStructArgMember( 0, "Key", psKey )
         && args->getStructArgMember( 0, "Value", psValue )
         && args->getStructArgMember( 0, "State", psState )
      )
      {
         int stemp;
         QString pub;
         if (
            psPublisher->getString(pub) &&
            psServer->getString( server ) &&
            psCategory->getString( category ) &&
            psKey->getString( key ) &&
            psValue->getString( value ) &&
            psState->getInt( stemp )
         )
         {
            QStringList p = pub.split(QChar('@'), QString::KeepEmptyParts);
            if (p.size() > 1)
            {
                publisherServer = p[1];
            }
            if (publisherServer == "localhost")
            {
                publisherServer = MinosConfig::getMinosConfig()->getThisServerName();
            }
            publisherProgram = p[0];
            state = static_cast<PublishState>(stemp);
            OK = true;
         }
      }
   }
}
