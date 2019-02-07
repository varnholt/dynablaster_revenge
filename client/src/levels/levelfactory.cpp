// header
#include "levelfactory.h"

// levels
#include "castle/levelcastle.h"
#include "mansion/levelmansion.h"
#include "space/levelspace.h"

// game
#include "constants.h"

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
      case Level::LevelMansion:
         level = new LevelMansion();
         break;
      case Level::LevelSpace:
         level = new LevelSpace();
         break;

      default:
         break;
   }

   return level;
}


Level *LevelFactory::getLevelInstance(const QString &levelName)
{
   Level::LevelType levelType = Level::LevelCastle;

   if (levelName.endsWith(Level::getLevelDirectoryName(Level::LevelCastle)))
      levelType = Level::LevelCastle;
   else if (levelName.endsWith(Level::getLevelDirectoryName(Level::LevelMansion)))
      levelType = Level::LevelMansion;
   else if (levelName.endsWith(Level::getLevelDirectoryName(Level::LevelSpace)))
      levelType = Level::LevelSpace;
   else if (levelName.endsWith(Level::getLevelDirectoryName(Level::LevelDummy)))
      levelType = Level::LevelDummy;

   return getLevelInstance(levelType);
}


