#ifndef ASTARMAP_H
#define ASTARMAP_H

// base
#include "botmap.h"

// Qt
#include <QList>

// forward declarations
class AStarNode;

class AStarMap : public BotMap
{
   public:

      //! constructor
      AStarMap();

      //! constructor
      AStarMap(int width, int height);

      //! copy constructor for botmap (map items are not copied)
      explicit AStarMap(BotMap*);

      //! copy constructor for astar map (map items are not copied)
      explicit AStarMap(AStarMap*);

      //! destructor
      virtual ~AStarMap();

      //! clear map
      void clear();

      //! setter for botmap
      void setBotMap(BotMap *);

      //! create nodes
      void buildNodes();

      //! delete all nodes
      void clearNodes();

      //! get all neighbor nodes of a position
      QList<AStarNode*> getNeighbors(int x, int y, bool regardStones = false);

      //! get node at x, y
      AStarNode* getNode(
         int x,
         int y
      ) const;

      //! set node at x, y
      void setNode(
         int x,
         int y,
         AStarNode*
      );

      //! debug path
      void debugPath(const QList<AStarNode*>& path);


   protected:

      //! init map
      void initMap();

      //! check if a point is traversable
      bool isTraversable(const QPoint& p, bool regardStones);

      //! node map
      AStarNode** mNodeMap;

      //! list of nodes
      QList<AStarNode*> mNodes;
};

#endif // ASTARMAP_H


