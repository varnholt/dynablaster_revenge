#ifndef LEVELFACTORY_H
#define LEVELFACTORY_H

// game
#include "level.h"

// Qt
#include <QString>

class LevelFactory
{
   public:

      LevelFactory();

      static LevelFactory* getFactoryInstance();

      static Level* getLevelInstance(Level::LevelType type);
      static Level* getLevelInstance(const QString& typeName);

   private:

      static LevelFactory* sInstance;
};

#endif // LEVELFACTORY_H
