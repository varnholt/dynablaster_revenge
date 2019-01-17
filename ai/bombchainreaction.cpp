// header
#include "bombchainreaction.h"

// ai
#include "botmap.h"


//-----------------------------------------------------------------------------
/*!
*/
BombChainReaction::BombChainReaction()
 : mBotMap(0)
{
}


//-----------------------------------------------------------------------------
/*!
   \param map map to set
*/
void BombChainReaction::setBotMap(BotMap *map)
{
   mBotMap = map;
}


//-----------------------------------------------------------------------------
/*!
*/
void BombChainReaction::compute()
{
   // clear old chain data
   mChain.clear();
   mVisited.clear();

   // list all bombs
   QList<BotBombMapItem *> bombs = mBotMap->getBombs();

   // iterate through all bombs in case their not visited
   foreach (BotBombMapItem* item, bombs)
   {
      if (!mVisited.contains(item))
      {
         // build a detonation chain and append it in case it's not empty
         QList<BotBombMapItem*> items;

         iterate(item, items);

         if (!items.isEmpty())
            mChain.append(items);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param item root item
   \param items list to add items to
*/
void BombChainReaction::iterate(
   BotBombMapItem *item,
   QList<BotBombMapItem*>& items
)
{
   mVisited.insert(item);
   items.append(item);

   // check which bombs are hit by the given bomb
   int x = 0;
   int y = 0;
   int xi = 0;
   int yi = 0;

   x = item->getX();
   y = item->getY();

   foreach (const QPoint& dir, mDirections)
   {
      for (int i = 1; i <= item->getFlames(); i++)
      {
         xi = x + i * dir.x();
         yi = y + i * dir.y();

         if (
               xi >= 0 && xi < mBotMap->getWidth()
            && yi >= 0 && yi < mBotMap->getHeight()
         )
         {
            MapItem* mapItem = mBotMap->getItem(xi, yi);

            if (mapItem)
            {
               if (mapItem->getType() == MapItem::Bomb)
               {
                  BotBombMapItem* hitBomb = dynamic_cast<BotBombMapItem*>(mapItem);

                  if (!mVisited.contains(hitBomb))
                  {
                     iterate(hitBomb, items);
                  }
               }

               // evaluating this direction ends here since we hit an item
               break;
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void BombChainReaction::unitTest1()
{
}


//-----------------------------------------------------------------------------
/*!
*/
void BombChainReaction::initDirections()
{
   mDirections.clear();
   mDirections << QPoint(0, -1);
   mDirections << QPoint(0, 1);
   mDirections << QPoint(-1, 0);
   mDirections << QPoint(1, 0);
}


//-----------------------------------------------------------------------------
/*!
   \return detonation chain
*/
const BombChainReaction::ChainList &BombChainReaction::getDetonationChain()
{
   return mChain;
}

