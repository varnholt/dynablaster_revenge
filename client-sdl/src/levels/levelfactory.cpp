// header
#include "levelfactory.h"

// levels
#include "castle/levelcastle.h"

// game
#include "constants.h"

// LevelMansion/LevelSpace not ported (see project memory - Phase 5 deferred scope) - only
// LevelCastle is reachable in practice (CreateGameSettings defaults LevelIndex to 0/castle).

LevelFactory* LevelFactory::sInstance = 0;


LevelFactory::LevelFactory()
{
   sInstance = this;
}


LevelFactory *LevelFactory::getFactoryInstance()
{
   return sInstance ? sInstance : new LevelFactory();
}


Level *LevelFactory::getLevelInstance(Level::LevelType levelType)
{
   Level* level = 0;

   switch (levelType)
   {
      case Level::LevelCastle:
         level = new LevelCastle();
         break;

      default:
         break;
   }

   return level;
}


Level *LevelFactory::getLevelInstance(const QString &levelName)
{
   Level::LevelType levelType = Level::LevelCastle;

   if (levelName.toStdString().ends_with(Level::getLevelDirectoryName(Level::LevelCastle)))
      levelType = Level::LevelCastle;

   return getLevelInstance(levelType);
}
