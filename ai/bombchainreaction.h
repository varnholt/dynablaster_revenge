#ifndef BOMBCHAINREACTION_H
#define BOMBCHAINREACTION_H

// ai
class BotMap;

// shared
#include "botbombmapitem.h"
#include "point.h"

// Qt
#include <QList>

#include <unordered_set>


class BombChainReaction
{
   public:

      typedef QList< QList<BotBombMapItem*> > ChainList;

      //! constructor
      BombChainReaction();

      //! setter for bot map
      void setBotMap(BotMap* map);

      //! compute chained bombs
      void compute();

      //! do unit test
      void unitTest1();

      //! initialize list of directions
      void initDirections();

      //! getter for detonation chain
      const ChainList& getDetonationChain();


   protected:

      //! recursion
      void iterate(BotBombMapItem *item, QList<BotBombMapItem *> &items);

      //! visited items
      std::unordered_set<BotBombMapItem*> mVisited;

      //! bot map
      BotMap* mBotMap;

      //! list of detonation chains
      ChainList mChain;

      //! direction vectors
      QList<Point> mDirections;
};

#endif // BOMBCHAINREACTION_H
