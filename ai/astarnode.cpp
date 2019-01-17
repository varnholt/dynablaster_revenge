// header
#include "astarnode.h"

// math
#include <stdlib.h>


//-----------------------------------------------------------------------------
/*!
   \param parent parent node
*/
AStarNode::AStarNode(AStarNode* parent)
   : mParent(parent),
     mX(0),
     mY(0),
     mG(0),
     mH(0),
     mF(0)
{
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
*/
void AStarNode::setX(int x)
{
   mX = x;
}


//-----------------------------------------------------------------------------
/*!
   \param y y position
*/
void AStarNode::setY(int y)
{
   mY = y;
}


//-----------------------------------------------------------------------------
/*!
   \param parent parent node
*/
void AStarNode::setParent(AStarNode* parent)
{
   mParent = parent;
}


//-----------------------------------------------------------------------------
/*!
   \return parent node
*/
AStarNode* AStarNode::getParent() const
{
   return mParent;
}

//-----------------------------------------------------------------------------
/*!
   \return x position
*/
int AStarNode::getX() const
{
   return mX;
}


//-----------------------------------------------------------------------------
/*!
   \return y position
*/
int AStarNode::getY() const
{
   return mY;
}


//-----------------------------------------------------------------------------
/*!
   \param g g value
*/
void AStarNode::setG(int g)
{
   mG = g;
}


//-----------------------------------------------------------------------------
/*!
   \return g value
*/
int AStarNode::getG() const
{
   return mG;
}


//-----------------------------------------------------------------------------
/*!
   \return h value
*/
int AStarNode::getH() const
{
   return mH;
}


//-----------------------------------------------------------------------------
/*!
   \return f value
*/
int AStarNode::getF() const
{
   return mF;
}


//-----------------------------------------------------------------------------
/*!
*/
void AStarNode::calcG()
{
   AStarNode* parent = mParent;

   while (parent)
   {
      mG++;
      parent = parent->mParent;
   }
}


//-----------------------------------------------------------------------------
/*!
   \param target target node
   \return distance to given node
*/
int AStarNode::getDistance(AStarNode *target)
{
   return abs(mX - target->getX()) + abs(mY + target->getY());;
}


//-----------------------------------------------------------------------------
/*!
   \param target target to calculate h to
*/
void AStarNode::calcH(AStarNode* target)
{
   mH = getDistance(target);
}


//-----------------------------------------------------------------------------
/*!
*/
void AStarNode::calcF()
{
   mF = mG + mH;
}

