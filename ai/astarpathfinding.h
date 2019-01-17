#ifndef ASTARPATHFINDING_H
#define ASTARPATHFINDING_H

#include <QList>
#include <QSet>

#include "pathfinding.h"

#include "map.h"

#include "astarmap.h"
#include "astarnode.h"

class AStarPathFinding : public PathFinding
{
   public:

      //! constructor
      AStarPathFinding();

      //! setter for map
      void setMap(AStarMap* map);

      //! setter for start pojt
      void setStart(int x, int y);

      //! setter for target point
      void setTarget(int x, int y);

      //! find path from start to target
      void findPath();

      //! debug path
      void debugPath();

      //! debug path in a short version
      void debugPathShort();

      //! getter for path nodes
      QList<AStarNode*> getPath() const;

      //! getter for the path length
      int getPathLength() const;


   protected:

      //! get best f score node in given set
      AStarNode* getBestFValueNode(QSet<AStarNode *> *set);

      //! build path by linking parents
      QList<AStarNode *> reconstructPath(AStarNode* currentNode);

      //! open set
      QSet<AStarNode*> mOpenSet;

      //! closed set
      QSet<AStarNode*> mClosedSet;

      //! start node
      AStarNode* mStartNode;

      //! target node
      AStarNode* mTargetNode;

      //! current node
      AStarNode* mCurrentNode;

      //! map to work on
      AStarMap* mNodeMap;

      //! resulting path
      QList<AStarNode*> mPath;
};

#endif // ASTARPATHFINDING_H



/*

   a* pathfinding algorithm


   input:
   - start node
   - end node

   item states:
   - walkable
   - non-walkable

   +-----------+
   | g       h |
   |           |
   |     i     |
   |           |
   | f         |
   +-----------+

   g-score: number of parent nodes (length of the path)

   h-score: number of steps between current pos
            and target in steps in vertical or horizontal
            direction (manhattan distance, heuristic value)

   f-score: g-score + h-score

   i: processing index (lower index goes first if tiles have same score)


   Part 1: http://www.youtube.com/watch?v=Kw8AMmyc6vg
   Part 2: http://www.youtube.com/watch?v=uIVu7ViLaZo
   Part 3: http://www.youtube.com/watch?v=kcYkd1Oxnrc


   http://wiki.gamegardens.com/Path_Finding_Tutorial


   http://en.wikipedia.org/wiki/A*_search_algorithm

*/
