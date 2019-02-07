#include "helpmanager.h"

HelpManager* HelpManager::sInstance = 0;


HelpManager::HelpManager()
 : QObject()
{
}


HelpManager* HelpManager::getInstance()
{
   if (!sInstance)
   {
      sInstance = new HelpManager();
   }

   return sInstance;
}


void HelpManager::addMessage(
   const QString& page,
   const QString &message,
   Constants::HelpSeverity severity,
   Constants::HelpLocation location,
   int delay
)
{
   emit messageAdded(
      page,
      message,
      severity,
      location,
      delay
   );
}


