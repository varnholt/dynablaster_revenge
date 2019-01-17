// header
#include "astarpathfinding.h"

//-----------------------------------------------------------------------------
/*!
*/
AStarPathFinding::AStarPathFinding()
 : mStartNode(0),
   mTargetNode(0),
   mCurrentNode(0),
   mNodeMap(0)
{
}

//-----------------------------------------------------------------------------
/*!
   \param map astar map to process
*/
void AStarPathFinding::setMap(AStarMap* map)
{
   mNodeMap = map;
}


//-----------------------------------------------------------------------------
/*!
   \param start x position
   \param start y position
*/
void AStarPathFinding::setStart(int x, int y)
{
   mStartNode = mNodeMap->getNode(x, y);
}


//-----------------------------------------------------------------------------
/*!
   \param target x position
   \param target y position
*/
void AStarPathFinding::setTarget(int x, int y)
{
   mTargetNode = mNodeMap->getNode(x, y);
}


//-----------------------------------------------------------------------------
/*!
*/
void AStarPathFinding::findPath()
{
   // init
   int testGScore = 0;
   bool testBetter = false;

   // reset
   mPath.clear();
   mOpenSet.clear();
   mClosedSet.clear();

   // add starting node to open list
   if (mStartNode)
      mOpenSet.insert(mStartNode);

   while (!mOpenSet.isEmpty())
   {
      // consider the best node in the open list (the node with the lowest f value)
      // the node in openset having the lowest f_score[] value;
      mCurrentNode = getBestFValueNode(&mOpenSet);

      // this node is the goal
      if (
            mCurrentNode->getX() == mTargetNode->getX()
         && mCurrentNode->getY() == mTargetNode->getY()
      )
      {
         // then we're done
         mOpenSet.clear();
         mPath = reconstructPath(mCurrentNode);
      }
      else
      {
         // remove current from openset
         mOpenSet.remove(mCurrentNode);

         // add current to closedset
         mClosedSet.insert(mCurrentNode);

         // for (each neighbor) // i.e. up, down, left, right
         foreach (
            AStarNode* neighbor,
            mNodeMap->getNeighbors(mCurrentNode->getX(), mCurrentNode->getY(), true)
         )
         {
            if (!mClosedSet.contains(neighbor))
            {
               testBetter = false;
               mCurrentNode->calcG();
               testGScore = mCurrentNode->getG() + mCurrentNode->getDistance(mTargetNode);

               if (!mOpenSet.contains(neighbor))
               {
                  mOpenSet.insert(neighbor);
                  neighbor->calcH(mTargetNode);
                  testBetter = true;
               }
               else
               {
                  neighbor->calcG();
                  testBetter = (testGScore < neighbor->getG());
               }

               if (testBetter)
               {
                  neighbor->setParent(mCurrentNode);
                  neighbor->setG(testGScore);
                  neighbor->calcF();
               }
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param currentNode
   \return all parent nodes
*/
QList<AStarNode*> AStarPathFinding::reconstructPath(AStarNode* currentNode)
{
   QList<AStarNode*> path;

   while (currentNode && currentNode->getParent())
   {
      path.push_back(currentNode);
      currentNode = currentNode->getParent();
   }

   return path;
}


//-----------------------------------------------------------------------------
/*!
   \param set set to scan
   \return node with best f value
*/
AStarNode* AStarPathFinding::getBestFValueNode(QSet<AStarNode*>* set)
{
   AStarNode* node = 0;

   int fMin = INT_MAX;

   foreach (AStarNode* n, *set)
   {
      if (n->getF() < fMin)
      {
         node = n;
         fMin = node->getF();
      }
   }

   return node;
}


//-----------------------------------------------------------------------------
/*!
*/
void AStarPathFinding::debugPath()
{
   int i = 0;
   foreach (AStarNode* node, mPath)
   {
      qDebug("%d: (%d, %d)", i, node->getX(), node->getY());
      i++;
   }
}


//-----------------------------------------------------------------------------
/*!
  \param path path to debug
*/
void AStarPathFinding::debugPathShort()
{
   QString pathString;

   foreach(AStarNode* node, mPath)
   {
      QString point =
         QString("=> (%1; %2) ")
            .arg(node->getX())
            .arg(node->getY());

      pathString.append(point);
   }

   if (!pathString.isEmpty())
   {
      qDebug("AStarMap::debugPathSimplified: %s", qPrintable(pathString));
   }
   else
   {
      qDebug("AStarMap::debugPathSimplified: path is empty");
   }
}


//-----------------------------------------------------------------------------
/*!
   \return computed path
*/
QList<AStarNode*> AStarPathFinding::getPath() const
{
   return mPath;
}

//-----------------------------------------------------------------------------
/*!
   \return computed path length
*/
int AStarPathFinding::getPathLength() const
{
   return mPath.size();
}



/*
 wiki pseudocode

function A*(start,goal)
     closedset := the empty set    // The set of nodes already evaluated.
     openset := {start}    // The set of tentative nodes to be evaluated, initially containing the start node
     came_from := the empty map    // The map of navigated nodes.

     g_score[start] := 0    // Cost from start along best known path.
     h_score[start] := heuristic_cost_estimate(start, goal)
     f_score[start] := g_score[start] + h_score[start]    // Estimated total cost from start to goal through y.

     while openset is not empty
         current := the node in openset having the lowest f_score[] value
         if current = goal
             return reconstruct_path(came_from, came_from[goal])

         remove current from openset
         add current to closedset

         for each neighbor in neighbor_nodes(current)
         {
             if neighbor in closedset
                 continue

             tentative_g_score := g_score[current] + dist_between(current,neighbor)

             if neighbor not in openset
                 add neighbor to openset
                 h_score[neighbor] := heuristic_cost_estimate(neighbor, goal)
                 tentative_is_better := true
             else if tentative_g_score < g_score[neighbor]
                 tentative_is_better := true
             else
                 tentative_is_better := false

             if tentative_is_better = true
                 came_from[neighbor] := current
                 g_score[neighbor] := tentative_g_score
                 f_score[neighbor] := g_score[neighbor] + h_score[neighbor]
         }

     return failure

 function reconstruct_path(came_from, current_node)
     if came_from[current_node] is set
         p := reconstruct_path(came_from, came_from[current_node])
         return (p + current_node)
     else
         return current_node
*/


