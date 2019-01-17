// header
#include "bombkickanimation.h"

// shared
#include "bombmapitem.h"
#include "constants.h"
#include "map.h"
#include "mapitem.h"

// math
#include <math.h>

// Qt
#include <QTimer>

// defines
#define BOMB_MOVE_SPEED 3.0f
#define BOMB_EXPLODE_EPSILON 0.2f

// static
QList<BombKickAnimation*> BombKickAnimation::sAnimations;

//-----------------------------------------------------------------------------
/*!
   \param parent parent object
*/
BombKickAnimation::BombKickAnimation(QObject *parent)
  : QObject(parent),
    mTimer(0),
    mFactor(BOMB_MOVE_SPEED),
    mDirection(Constants::DirectionUnknown),
    mX(0.0f),
    mY(0.0f),
    mReadyToExplode(false),
    mMap(0),
    mBombMapItem(0),
    mColliding(false)
{
   mTimer = new QTimer(this);
   mTimer->setInterval(1000 / SERVER_HEARTBEAT_IN_HZ);

   connect(
      mTimer,
      SIGNAL(timeout()),
      this,
      SLOT(updatePosition())
   );

   addAnimation(this);
}


//-----------------------------------------------------------------------------
/*!
*/
BombKickAnimation::~BombKickAnimation()
{
   removeAnimation(this);
}


//-----------------------------------------------------------------------------
/*!
*/
void BombKickAnimation::deleteAll()
{
   while (!sAnimations.empty())
      delete sAnimations.first();
}


//-----------------------------------------------------------------------------
/*!
*/
void BombKickAnimation::start()
{   
   reset();

   unmapBomb();

   if (!mTimer->isActive())
      mTimer->start();

   emit started(
      getDirection(),
      getStepSize()
   );
}


//-----------------------------------------------------------------------------
/*!
   \param map ptr to map
*/
void BombKickAnimation::setMap(Map *map)
{
   mMap = map;
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
*/
void BombKickAnimation::setX(float x)
{
   mX = x;
}


//-----------------------------------------------------------------------------
/*!
   \param y y position
*/
void BombKickAnimation::setY(float y)
{
   mY = y;
}


//-----------------------------------------------------------------------------
/*!
   \return x position
*/
float BombKickAnimation::getX() const
{
   return mX;
}


//-----------------------------------------------------------------------------
/*!
   \return y position
*/
float BombKickAnimation::getY() const
{
   return mY;
}


//-----------------------------------------------------------------------------
/*!
   \return ptr to map
*/
Map *BombKickAnimation::getMap() const
{
   return mMap;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if ready to explode
*/
bool BombKickAnimation::isReadyToExplode() const
{
   return mReadyToExplode;
}


//-----------------------------------------------------------------------------
/*!
   \param read \c true if bomb is ready to explode
*/
void BombKickAnimation::setReadyToExplode(bool ready)
{
   mReadyToExplode = ready;
}


//-----------------------------------------------------------------------------
/*!
   \param direction dir
*/
void BombKickAnimation::setDirection(Constants::Direction dir)
{
   mDirection = dir;
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
   \param y y position
*/
void BombKickAnimation::ignite(int x, int y)
{
   foreach (BombKickAnimation* anim, sAnimations)
   {
      int ax = (int)floor(anim->getX());
      int ay = (int)floor(anim->getY());

      if (ax == x && ay == y)
      {
         // set the "ready" to explode flag.
         // it is reasonable to use the setter here instead of the slot
         // since ignite(...) could be called more than 1 time. so we just
         // leave the bomb in 'it can detonate every second'-state until
         // it stays in the middle of a field.
         anim->setReadyToExplode(true);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void BombKickAnimation::unmapBomb()
{
   int x = 0;
   int y = 0;

   x = (int)floor(mX);
   y = (int)floor(mY);

   mBombMapItem = dynamic_cast<BombMapItem*>(getMap()->getItem(x, y));

   // either clear the field or re-set the shadowed item
   getMap()->setItem(
      x,
      y,
      mBombMapItem->getShadowedItem()
   );

   /*
   qDebug(
      "BombKickAnimation::unmapBomb(): unmapping %p from %d, %d",
      mBombMapItem,
      x,
      y
   );
   */
}


//-----------------------------------------------------------------------------
/*!
*/
void BombKickAnimation::remapBomb()
{
   int x = 0;
   int y = 0;

   x = (int)floor(mX);
   y = (int)floor(mY);

   mBombMapItem->setX(x);
   mBombMapItem->setY(y);

   // item may have shadowed an extra
   mBombMapItem->setShadowedItem(
      getMap()->getItem(
         x,
         y
      )
   );

   getMap()->setItem(
      x,
      y,
      mBombMapItem
   );

   /*
   qDebug(
      "BombKickAnimation::remapBomb(): remapping %p to %d, %d",
      mBombMapItem,
      x,
      y
   );
   */
}


//-----------------------------------------------------------------------------
/*!
*/
void BombKickAnimation::reset()
{
   mReadyToExplode = false;
}


//-----------------------------------------------------------------------------
/*!
   \param value1 1st value
   \param value2 2nd value
   \param epsilon allowed tolerance
   \return \c true if value is in range
*/
bool BombKickAnimation::isInRange(float value1, float value2, float epsilon)
{
   bool inRange = false;
   inRange = (fabs(value1 - value2) < epsilon);
   return inRange;
}


//-----------------------------------------------------------------------------
/*!
   \return direction
*/
Constants::Direction BombKickAnimation::getDirection() const
{
   return mDirection;
}


//-----------------------------------------------------------------------------
/*!
   \return x direction
*/
int BombKickAnimation::getDirectionX() const
{
   int dir = 0;

   switch (getDirection())
   {
      case Constants::DirectionLeft:
         dir = -1;
         break;
      case Constants::DirectionRight:
         dir = 1;
         break;

      default:
         break;
   }

   return dir;
}


//-----------------------------------------------------------------------------
/*!
   \return y direction
*/
int BombKickAnimation::getDirectionY() const
{
   int dir = 0;

   switch (getDirection())
   {
      case Constants::DirectionUp:
         dir = -1;
         break;
      case Constants::DirectionDown:
         dir = 1;
         break;

      default:
         break;
   }

   return dir;
}


//-----------------------------------------------------------------------------
/*!
*/
void BombKickAnimation::updatePosition()
{
   updateCollisions();

   if (isMoveAllowed() && !isColliding())
   {
      // update the bombs position
      mX += getDirectionX() * getStepSize();
      mY += getDirectionY() * getStepSize();

      // check if bomb may already exlode
      if (isReadyToExplode())
      {
         // check if bomb is finally in the center of a field so it can explode
         float fieldX = floor(mX);
         float fieldY = floor(mY);

         if (
               isInRange(fabs(mX - fieldX), 0.5f, BOMB_EXPLODE_EPSILON)
            && isInRange(fabs(mY - fieldY), 0.5f, BOMB_EXPLODE_EPSILON)
         )
         {
            // reposition the bomb
            remapBomb();

            // let it explode
            emit explode();
         }
      }

      // qDebug("BombKickAnimation::updatePosition(): moved to %f, %f", mX, mY);
   }
   else
   {
      // reposition the bomb
      remapBomb();

      // if bomb can't move tell client to stop the animation
      emit stopped();

      // let it explode
      readyToExplode();

      // no more updates required, just wait until this
      // object instance is deleted when bomb explodes
      mTimer->stop();
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void BombKickAnimation::readyToExplode()
{
   // qDebug("BombKickAnimation::readyToExplode(): %d", mReadyToExplode);

   // if bomb *is* already "ready to explode", then explode
   // otherwise wait for it to be in its final position
   if (mReadyToExplode)
   {
      emit explode();
   }
   else
   {
      mReadyToExplode = true;
   }
}


//-----------------------------------------------------------------------------
/*!
   \param id player id
   \param x player x position
   \param y player y position
*/
void BombKickAnimation::updatePlayerPosition(int id, float x, float y)
{
   mPlayerPositions[id] = QPoint((int)floor(x), (int)floor(y));
}


//-----------------------------------------------------------------------------
/*!
   \param id player id
*/
void BombKickAnimation::removePlayerPosition(int id)
{
   mPlayerPositions.remove(id);
}


//-----------------------------------------------------------------------------
/*!
   \return true if movement is allowed
*/
bool BombKickAnimation::isMoveAllowed()
{
   bool allowed = true;

   // make field position of x and y
   int fieldXPos = floor(getX());
   int fieldYPos = floor(getY());

   int checkFieldX = 0;
   int checkFieldY = 0;

   switch (getDirection())
   {
      case Constants::DirectionLeft:
         checkFieldX = (int)floor(getX() - 0.5f - getStepSize());
         checkFieldY = fieldYPos;
         break;
      case Constants::DirectionRight:
         checkFieldX = (int)floor(getX() + 0.5f + getStepSize());
         checkFieldY = fieldYPos;
         break;
      case Constants::DirectionUp:
         checkFieldX = fieldXPos;
         checkFieldY = (int)floor(getY() - 0.5f - getStepSize());
         break;
      case Constants::DirectionDown:
         checkFieldX = fieldXPos;
         checkFieldY = (int)floor(getY() + 0.5f + getStepSize());
         break;
      default:
         break;
   }

   // block if map bounds are exceeded
   // or if bomb hit something
   if (  checkFieldX < 0
      || checkFieldY < 0
      || checkFieldX > getMap()->getWidth() - 1
      || checkFieldY > getMap()->getHeight() - 1
   )
   {
      allowed = false;
   }
   else
   {
      // check for map item collisions
      MapItem* item = getMap()->getItem(checkFieldX, checkFieldY);

      if (
            item
         && item->isBlocking()
      )
      {
         allowed = false;
      }
      else
      {
         // if there is no blocking item, we still need to check for
         // player collisions
         foreach(const QPoint& p, mPlayerPositions)
         {
            if (
                  p.x() == checkFieldX
               && p.y() == checkFieldY
            )
            {
               allowed = false;
               break;
            }
         }

         // of COURSE there is an exception.. when there's a player who
         // picked up a couple of speedups and he's running towards the
         // bomb kick direction, bomb and player might just overlap for
         // the time of one server heartbeat. the result is simply a bomb
         // moving "through" a player.
         if (allowed)
         {
            // we do not do this for the field the bomb was kicked from
            // just when the bomb is 'on its way'.
            int startX = mBombMapItem->getX();
            int startY = mBombMapItem->getY();

            if (
                  fieldXPos != startX
               || fieldYPos != startY
            )
            {
               // check if there's a player on the field the bomb is
               // at *this very moment*. if that is the case, well...
               // stop the animation.
               foreach(const QPoint& p, mPlayerPositions)
               {
                  if (
                        p.x() == fieldXPos
                     && p.y() == fieldYPos
                  )
                  {
                     allowed = false;
                     break;
                  }
               }
            }
         }
      }
   }

   return allowed;
}


//-----------------------------------------------------------------------------
/*!
   \return step size
*/
float BombKickAnimation::getStepSize() const
{
   return mFactor * SERVER_SPEED;
}


//-----------------------------------------------------------------------------
/*!
   \param animation animation to add
*/
void BombKickAnimation::addAnimation(BombKickAnimation *animation)
{
   sAnimations.push_back(animation);
}


//-----------------------------------------------------------------------------
/*!
   \param animation animation to remove
*/
void BombKickAnimation::removeAnimation(BombKickAnimation *animation)
{
   sAnimations.removeOne(animation);
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if bomb collides with another bomb
*/
bool BombKickAnimation::checkCollision(BombKickAnimation *animation)
{
   bool colliding = false;   

   float eps = 1.0f + 2.0f * getStepSize();
   bool checkVertical = false;
   bool checkHorizontal = false;

   // concept for simple horizontal/vertical checks:
   // if they get near enough (distance in x or y below eps)
   // then they're colliding. eps is greater than 1 because it is
   // necessary to ensure both collisions end on two different
   // fields.
   switch (getDirection())
   {
      case Constants::DirectionUp:
         checkVertical = (animation->getDirection() == Constants::DirectionDown);
         break;

      case Constants::DirectionDown:
         checkVertical = (animation->getDirection() == Constants::DirectionUp);
         break;

      case Constants::DirectionLeft:
         checkHorizontal = (animation->getDirection() == Constants::DirectionRight);
         break;

      case Constants::DirectionRight:
         checkHorizontal = (animation->getDirection() == Constants::DirectionLeft);
         break;

      default:
         break;
   }

   if (checkVertical)
   {
      if ( (int)floor(getX()) == (int)floor(animation->getX()) )
      {
         if ( fabs( getY() - animation->getY() ) < eps )
            colliding = true;
      }
   }

   else if (checkHorizontal)
   {
      if ( (int)floor(getY()) == (int)floor(animation->getY()) )
      {
         if ( fabs( getX() - animation->getX() ) < eps )
            colliding = true;
      }
   }

   // check diagonal collisions as well... yes, fuck murphy :)
   // concept for diagonal collision checks:
   // shift both running animations along their directions.
   // this shifting is done for each direction individually, i.e.
   // - shift both animation
   // - shift x or/and y of animation 1
   // - shift x or/and y of animation 2
   if (!colliding)
   {
      /*
         +---+---+         +---+
         |   |///|         |///|
         +---+---+ lu      +---+---+ ld
         |///|             |   |///|
         +---+             +---+---+


         +---+---+             +---+
         |///|   |             |///|
         +---+---+ ru      +---+---+ rd
             |///|         |///|   |
             +---+         +---+---+

         ---------------------------

         +---+             +---+---+
         |///|             |///|   |
         +---+---+ dl      +---+---+ ur
         |   |///|             |///|
         +---+---+             +---+


         +---+---+             +---+
         |   |///|             |///|
         +---+---+ ul      +---+---+ dr
         |///|             |///|   |
         +---+             +---+---+
      */

      // init
      float diagonalEps = 2.0f * getStepSize();

      float x1 = getX();
      float y1 = getY();
      float x2 = animation->getX();
      float y2 = animation->getY();

      int xField1 = floor(getX());
      int yField1 = floor(getY());
      int xField2 = floor(animation->getX());
      int yField2 = floor(animation->getY());

      int xFieldShifted1 = -1;
      int yFieldShifted1 = -1;
      int xFieldShifted2 = -1;
      int yFieldShifted2 = -1;

      float x1add = 0.0f;
      float y1add = 0.0f;
      float x2add = 0.0f;
      float y2add = 0.0f;

      // shift positions into possible collision range

      // lu
      if (
                       getDirection() == Constants::DirectionLeft
         && animation->getDirection() == Constants::DirectionUp
      )
      {
         x1add = -diagonalEps;
         y2add = -diagonalEps;
      }

      // ld
      else if (
                       getDirection() == Constants::DirectionLeft
         && animation->getDirection() == Constants::DirectionDown
      )
      {
         x1add = -diagonalEps;
         y2add = diagonalEps;
      }

      // ru
      else if (
                       getDirection() == Constants::DirectionRight
         && animation->getDirection() == Constants::DirectionUp
      )
      {
         x1add = diagonalEps;
         y2add = -diagonalEps;
      }

      // rd
      else if (
                       getDirection() == Constants::DirectionRight
         && animation->getDirection() == Constants::DirectionDown
      )
      {
         x1add = diagonalEps;
         y2add = diagonalEps;
      }

      // ---

      // dl
      else if (
                       getDirection() == Constants::DirectionDown
         && animation->getDirection() == Constants::DirectionLeft
      )
      {
         y1add = diagonalEps;
         x2add = -diagonalEps;
      }

      // ul
      else if (
                       getDirection() == Constants::DirectionUp
         && animation->getDirection() == Constants::DirectionLeft
      )
      {
         y1add = -diagonalEps;
         x2add = -diagonalEps;
      }

      // ur
      else if (
                       getDirection() == Constants::DirectionUp
         && animation->getDirection() == Constants::DirectionRight
      )
      {
         y1add = -diagonalEps;
         x2add = diagonalEps;
      }

      // dr
      else if (
                       getDirection() == Constants::DirectionDown
         && animation->getDirection() == Constants::DirectionRight
      )
      {
         y1add = diagonalEps;
         x2add = diagonalEps;
      }

      // add modified epsilons to every position
      x1 += x1add;
      y1 += y1add;
      x2 += x2add;
      y2 += y2add;

      // init fields
      xFieldShifted1 = floor(x1);
      yFieldShifted1 = floor(y1);
      xFieldShifted2 = floor(x2);
      yFieldShifted2 = floor(y2);

      // check if fields are in collision range
      colliding =
            (xFieldShifted1 == xFieldShifted2 && yFieldShifted1 == yFieldShifted2)  // all shifted
                                                                                    //
         || (xFieldShifted1 == xField2        && yFieldShifted1 == yField2)         // animation 1 shifted
         || (xFieldShifted1 == xField2        && yField1        == yField2)         // animation 1 x shifted
         || (xField1        == xField2        && yFieldShifted1 == yField2)         // animation 1 y shifted
                                                                                    //
         || (xField1        == xFieldShifted2 && yField1        == yFieldShifted2)  // animation 2 shifted
         || (xField1        == xFieldShifted2 && yField1        == yField2)         // animation 2 x shifted
         || (xField1        == xField2        && yField1        == yFieldShifted2); // animation 2 x shifted
   }

   return colliding;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if bomb is colliding
*/
bool BombKickAnimation::isColliding() const
{
   return mColliding;
}


//-----------------------------------------------------------------------------
/*!
   \param colliding colliding flag
*/
void BombKickAnimation::setColliding(bool colliding)
{
   mColliding = colliding;
}


//-----------------------------------------------------------------------------
/*!
*/
void BombKickAnimation::updateCollisions()
{
   // check our current animation vs. all animations that are currently active
   foreach (BombKickAnimation* animation, sAnimations)
   {
      if (animation != this)
      {
         // if the other kicked bomb collides with this kicked bomb, we set
         // both kicked bombs to 'colliding' -> they're remapped in the next
         // step so they're stopped and placed next to each other.
         if (checkCollision(animation))
         {
            setColliding(true);
            animation->setColliding(true);
         }
      }
   }
}




