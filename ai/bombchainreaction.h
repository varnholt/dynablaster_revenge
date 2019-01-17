#ifndef BOMBCHAINREACTION_H
#define BOMBCHAINREACTION_H

// ai
class BotMap;

// shared
#include "botbombmapitem.h"

// Qt
#include <QPoint>
#include <QSet>


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
      QSet<BotBombMapItem*> mVisited;

      //! bot map
      BotMap* mBotMap;

      //! list of detonation chains
      ChainList mChain;

      //! direction vectors
      QList<QPoint> mDirections;
};

#endif // BOMBCHAINREACTION_H
