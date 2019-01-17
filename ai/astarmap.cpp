// header
#include "astarmap.h"

// astar
#include "astarnode.h"

// shared
#include "mapitem.h"

// Qt
#include <QStringList>


//-----------------------------------------------------------------------------
/*!
*/
AStarMap::AStarMap()
   : BotMap(),
     mNodeMap(0)
{
   initMap();
}


//-----------------------------------------------------------------------------
/*!
*/
AStarMap::AStarMap(int width, int height)
   : BotMap(width, height)
{
   initMap();
}


//-----------------------------------------------------------------------------
/*!
   \param map map to copy
*/
AStarMap::AStarMap(BotMap* map)
   : BotMap()
{
   // init map
   initMap();

   for (int x = 0; x < mWidth; x++)
   {
      for (int y = 0; y < mHeight; y++)
      {
         setItem(x, y, map->getItem(x, y));
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param map map to copy
*/
AStarMap::AStarMap(AStarMap* map)
{
   // init map
   initMap();

   for (int x = 0; x < mWidth; x++)
   {
      for (int y = 0; y < mHeight; y++)
      {
         setItem(x, y, map->getItem(x, y));
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
AStarMap::~AStarMap()
{
}


//-----------------------------------------------------------------------------
/*!
*/
void AStarMap::initMap()
{
   mNodeMap = new AStarNode*[mWidth*mHeight];
   memset(mNodeMap, 0, mWidth * mHeight * sizeof(AStarNode*));
}


//-----------------------------------------------------------------------------
/*!
   \param map botmap
*/
void AStarMap::setBotMap(BotMap* map)
{
   for (int x = 0; x < mWidth; x++)
      for (int y = 0; y < mHeight; y++)
         setItem(x, y, map->getItem(x, y));
}


//-----------------------------------------------------------------------------
/*!
*/
void AStarMap::buildNodes()
{
   for (int x = 0; x < mWidth; x++)
   {
      for (int y = 0; y < mHeight; y++)
      {
         // init node
         AStarNode* node = new AStarNode();
         node->setX(x);
         node->setY(y);

         // add node to map
         mNodeMap[y * mWidth + x] = node;

         // add node to node list
         mNodes.push_back(node);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void AStarMap::clearNodes()
{
   for (int x = 0; x < mWidth; x++)
   {
      for (int y = 0; y < mHeight; y++)
      {
         delete mNodeMap[y * mWidth + x];
         mNodeMap[y * mWidth + x] = 0;
      }
   }

   mNodes.clear();
}


//-----------------------------------------------------------------------------
/*!
  \param x x position
  \param y y position
  \param regardStones \c true if stones are to be regarded
  \return list of neighbors
*/
QList<AStarNode*> AStarMap::getNeighbors(int x, int y, bool regardStones)
{
   QList<AStarNode*> list;

   QPoint up(x, y-1);
   QPoint down(x, y+1);
   QPoint left(x-1, y);
   QPoint right(x+1, y);

   if (up.y() >= 0)
   {
      if (isTraversable(up, regardStones))
         list.push_back(getNode(up.x(), up.y()));
   }

   if (down.y() < getHeight())
   {
      if (isTraversable(down, regardStones))
         list.push_back(getNode(down.x(), down.y()));
   }

   if (left.x() >= 0)
   {
      if (isTraversable(left, regardStones))
         list.push_back(getNode(left.x(), left.y()));
   }

   if (right.x() < getWidth())
   {
      if (isTraversable(right, regardStones))
         list.push_back(getNode(right.x(), right.y()));
   }

   return list;
}


//-----------------------------------------------------------------------------
/*!
  \param point point to check
  \param regardStones \c if stones are to be regarded
  \return true if point is traversable
*/
bool AStarMap::isTraversable(
   const QPoint& point,
   bool regardStones
)
{
   MapItem* item = 0;
   bool add = true;

   item = getItem(point.x(), point.y());

   if (item && item->getType() == MapItem::Bomb)
      add = false;

   if (item && item->getType() == MapItem::Block)
      add = false;

   if (add && regardStones && item && item->getType() == MapItem::Stone)
      add = false;

   return add;
}


//-----------------------------------------------------------------------------
/*!
   \return pointer to mapitem
   \param x x position
   \param y y position
*/
AStarNode* AStarMap::getNode(int x, int y) const
{
   return mNodeMap[y * mWidth + x];
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
   \param y y position
   \param mapitem
*/
void AStarMap::setNode(int x, int y, AStarNode* item)
{
   mNodeMap[y * mWidth + x]=item;
}


//-----------------------------------------------------------------------------
/*!
  \param path path to debug
*/
void AStarMap::debugPath(const QList<AStarNode *> &path)
{
   AStarNode** map = new AStarNode*[mWidth*mHeight];

   memset(map, 0, mWidth * mHeight * sizeof(AStarNode*));

   foreach (AStarNode* node, path)
      map[node->getY() * mWidth + node->getX()] = node;

   QStringList lines;
   MapItem* item = 0;
   QChar c;

   for (int yi = 0; yi < getHeight(); yi++)
   {
      QString line = QString("%1| ").arg(QString::number(yi, 16));

      for (int xi = 0; xi < getWidth(); xi++)
      {
         if (map[yi * mWidth + xi])
         {
            c = 'x';
         }
         else
         {
            item = getItem(xi, yi);

            if (item)
            {
               switch (item->getType())
               {
                  case MapItem::Block:
                     c = '#';
                     break;
                  case MapItem::Bomb:
                     c = 'B';
                     break;
                  case MapItem::Extra:
                     c = 'E';
                     break;
                  case MapItem::Stone:
                     c = 'S';
                     break;
                  case MapItem::Unknown:
                  default:
                     c = ' ';
                     break;
               }
            }
            else
            {
               c = ' ';
            }
         }

         line.append(c);
      }

      lines << line;
   }

   qDebug(" | 012345678901234567890123456789");
   qDebug(" +---------------");
   qDebug("%s", qPrintable(lines.join("\n")));
   qDebug(" +---------------");
   qDebug(" | 012345678901234567890123456789");

   delete[] map;
}



