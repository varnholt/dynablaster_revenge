// header
#include "collisiondetection.h"

// server
#include "game.h"

// shared
#include "map.h"
#include "mapitem.h"
#include "player.h"

// math
#include "math.h"



#define SERVER_CENTER_EPSILON       0.2f
#define SERVER_CORNER_EPSILON_MINOR 0.30f
#define SERVER_CORNER_EPSILON_MAJOR 0.70f


CollisionDetection::CollisionDetection(QObject *parent)
 : QObject(parent),
   mGame(0)
{
}


void CollisionDetection::process(Player *player)
{
   int keysPressed = player->getKeysPressed();
   bool moved = false;
   float desiredXPos = player->getX();
   float desiredYPos = player->getY();
   qint8 directions = 0;

   // ----------------------------------------------------------------------
   // drunken helli course correction (tm) mueslee 2012
   bool upPressed    = keysPressed & Constants::KeyUp;
   bool downPressed  = keysPressed & Constants::KeyDown;
   bool leftPressed  = keysPressed & Constants::KeyLeft;
   bool rightPressed = keysPressed & Constants::KeyRight;

   int field[2] = { (int)floor(player->getX()), (int)floor(player->getY()) };

   int left[2]  = { field[0] - 1, field[1]    };
   int right[2] = { field[0] + 1, field[1]    };
   int up[2]    = { field[0],     field[1] - 1};
   int down[2]  = { field[0],     field[1] + 1};

   bool fieldLeftAllowed   = !isFieldBlocked(left);
   bool fieldRightAllowed  = !isFieldBlocked(right);
   bool fieldUpAllowed     = !isFieldBlocked(up);
   bool fieldDownAllowed   = !isFieldBlocked(down);

   float absoluteX = player->getX();
   float absoluteY = player->getY();

   float relativeX = absoluteX - floor(absoluteX);
   float relativeY = absoluteY - floor(absoluteY);

   /*

             0.0, 0.0      1.0, 0.0      2.0, 0.0

   0.0, 0.0  +-------------+-------------+-------------+
             |             |             |             |
             |             |             |             |
             |             |             |             |
             |             |             |             |
             |             |             |             |
   0.0, 1.0  +---------<???+xxx>-----<xxx+???>---------+
             |             |/////////////| \
             |             |/////////////|  this part is handled (x)
             |             |/////////////|  1.5 ... 1.99999
             |             |/////////////|  => eventually check
             |             |/////////////|     2.0 ... 2.x, too
   0.0, 2.0  +-------------+-------------+     by adding 0.1?

   */

        if (downPressed &&! leftPressed && !rightPressed &&! upPressed && fieldDownAllowed && relativeX > SERVER_CORNER_EPSILON_MAJOR)
      keysPressed |= Constants::KeyLeft;
   else if (downPressed &&! leftPressed && !rightPressed &&! upPressed && fieldDownAllowed && relativeX < SERVER_CORNER_EPSILON_MINOR)
      keysPressed |= Constants::KeyRight;

   else if (upPressed && !leftPressed &&! rightPressed &&!downPressed && fieldUpAllowed && relativeX > SERVER_CORNER_EPSILON_MAJOR)
      keysPressed |= Constants::KeyLeft;
   else if (upPressed && !leftPressed &&! rightPressed &&!downPressed && fieldUpAllowed && relativeX < SERVER_CORNER_EPSILON_MINOR)
      keysPressed |= Constants::KeyRight;

   else if (leftPressed &&! rightPressed &&! upPressed &&! downPressed && fieldLeftAllowed && relativeY > SERVER_CORNER_EPSILON_MAJOR)
      keysPressed |= Constants::KeyUp;
   else if (leftPressed &&! rightPressed &&! upPressed &&! downPressed && fieldLeftAllowed && relativeY < SERVER_CORNER_EPSILON_MINOR)
      keysPressed |= Constants::KeyDown;

   else if (rightPressed &&! leftPressed &&! upPressed &&! downPressed && fieldRightAllowed && relativeY > SERVER_CORNER_EPSILON_MAJOR)
      keysPressed |= Constants::KeyUp;
   else if (rightPressed &&! leftPressed &&! upPressed &&! downPressed && fieldRightAllowed && relativeY < SERVER_CORNER_EPSILON_MINOR)
      keysPressed |= Constants::KeyDown;

   // ----------------------------------------------------------------------

   updatePlayerDirections(
      player,
      keysPressed,
      directions,
      desiredXPos,
      desiredYPos
   );

   // collision control
   int fieldX = floor(desiredXPos);
   int fieldY = floor(desiredYPos);
   float xInField = desiredXPos - floor(desiredXPos);
   float yInField = desiredYPos - floor(desiredYPos);
   float assignedXPos = 0.0;
   float assignedYPos = 0.0;
   bool playerMovesVertically = (player->getY() != desiredYPos);
   bool playerMovesHorizontally = (player->getX() != desiredXPos);
   bool horizontalMovementForbidden = false;
   bool verticalMovementForbidden = false;

   bool hBlocked = false;
   bool vBlocked = false;
   bool xInEpsilon = false;
   bool yInEpsilon = false;

   MapItem* kickedBomb1 = 0;
   MapItem* kickedBomb2 = 0;

   hBlocked =
      isPositionBlocked(
         desiredXPos,
         desiredYPos,
         keysPressed,
         true,
         fieldX,
         fieldY,
         &kickedBomb1
      );

   vBlocked =
      isPositionBlocked(
         desiredXPos,
         desiredYPos,
         keysPressed,
         false,
         fieldX,
         fieldY,
         &kickedBomb2
      );

   xInField = desiredXPos - floor(desiredXPos);
   yInField = desiredYPos - floor(desiredYPos);

   xInEpsilon =
         xInField <= 0.5 + SERVER_MOVE_EPSILON
      && xInField >= 0.5 - SERVER_MOVE_EPSILON;

   yInEpsilon =
         yInField <= 0.5 + SERVER_MOVE_EPSILON
      && yInField >= 0.5 - SERVER_MOVE_EPSILON;

   // kick bombs if possible
   emit playerKicksBomb(player, kickedBomb1, true, keysPressed);
   emit playerKicksBomb(player, kickedBomb2, false, keysPressed);

   // ----------------------------------------------------------------------
   // experimental position correction

   if (
         !xInEpsilon
      && !yInEpsilon
   )
   {
      // retry horizontal
      desiredXPos = player->getX();
      desiredYPos = player->getY();

      int keysFixedHorizontal = keysPressed;

      if (keysFixedHorizontal & Constants::KeyUp)
         keysFixedHorizontal &= ~(Constants::KeyUp);
      if (keysFixedHorizontal & Constants::KeyDown)
         keysFixedHorizontal &= ~(Constants::KeyDown);

      updatePlayerDirections(
         player,
         keysFixedHorizontal,
         directions,
         desiredXPos,
         desiredYPos
      );

      fieldX = floor(desiredXPos);
      fieldY = floor(desiredYPos);

      xInField = desiredXPos - floor(desiredXPos);
      yInField = desiredYPos - floor(desiredYPos);

      hBlocked =
         isPositionBlocked(
            desiredXPos,
            desiredYPos,
            keysFixedHorizontal,
            false,
            fieldX,
            fieldY
         );


      xInEpsilon =
            xInField <= 0.5 + SERVER_MOVE_EPSILON
         && xInField >= 0.5 - SERVER_MOVE_EPSILON;

      yInEpsilon =
            yInField <= 0.5 + SERVER_MOVE_EPSILON
         && yInField >= 0.5 - SERVER_MOVE_EPSILON;


      if (
            !xInEpsilon
         && !yInEpsilon
      )
      {
         desiredXPos = player->getX();
         desiredYPos = player->getY();

         int keysFixedVertical = keysPressed;

         if (keysFixedVertical & Constants::KeyRight)
            keysFixedVertical &= ~(Constants::KeyRight);
         if (keysFixedVertical & Constants::KeyLeft)
            keysFixedVertical &= ~(Constants::KeyLeft);

         updatePlayerDirections(
            player,
            keysFixedVertical,
            directions,
            desiredXPos,
            desiredYPos
         );

         fieldX = floor(desiredXPos);
         fieldY = floor(desiredYPos);

         // check if vertical movement is now possible
         vBlocked =
            isPositionBlocked(
               desiredXPos,
               desiredYPos,
               keysFixedVertical,
               true,
               fieldX,
               fieldY
            );
      }

      xInField = desiredXPos - floor(desiredXPos);
      yInField = desiredYPos - floor(desiredYPos);

      // x should be in epsilon now
      xInEpsilon =
            xInField <= 0.5 + SERVER_MOVE_EPSILON
         && xInField >= 0.5 - SERVER_MOVE_EPSILON;

      yInEpsilon =
            yInField <= 0.5 + SERVER_MOVE_EPSILON
         && yInField >= 0.5 - SERVER_MOVE_EPSILON;
   }

   // ----------------------------------------------------------------------


   // allow vertical movement only when
   // - position is changed
   // - x position is within 0.5 +- tolerance
   // - desired position is not blocked
   if (
         playerMovesVertically
      && xInEpsilon
      && !hBlocked
   )
   {
      // correct position until the player reached the middle of a field
      // or move the player to its desired position
      horizontalMovementForbidden =
         isPositionBlocked(
            desiredXPos,
            desiredYPos,
            keysPressed,
            false,
            fieldX,
            fieldY
         );

      // x position is above the movement-path
      assignedXPos =
         adjustXPosition(
            player,
            xInField,
            desiredXPos,
            playerMovesHorizontally,
            horizontalMovementForbidden
         );

      assignedYPos = desiredYPos;

      // yup, player was moved
      moved = true;
   }

   // allow horizontal movement only when
   // - position is changed
   // - x position is within 0.5 +- tolerance
   // - desired position is not blocked
   if (
         playerMovesHorizontally
      && yInEpsilon
      && !vBlocked
   )
   {
      // correct position until the player reached the middle of a field
      // or move the player to its desired position
      verticalMovementForbidden =
         isPositionBlocked(
            desiredXPos,
            desiredYPos,
            keysPressed,
            true,
            fieldX,
            fieldY
         );

      // y position is above the movement-path
      assignedYPos =
         adjustYPosition(
            player,
            yInField,
            desiredYPos,
            playerMovesVertically,
            verticalMovementForbidden
         );

      assignedXPos = desiredXPos;

      // yup, player was moved
      moved = true;
   }

   // update player rotation (if required)
   bool rotationChanged = updateRotation(
      player,
      keysPressed,
      moved,
      assignedXPos,
      assignedYPos
   );

   if (moved || rotationChanged)
   {
      int skipCount = player->getPositionSkipCounter();

      if (
            !moved // <- TODO: always set! why?
         || (getGame()->getState() != Constants::GameActive)
         || player->isKilled()
      )
      {
         assignedXPos = player->getX();
         assignedYPos = player->getY();
      }

      if (
            (player->getKeysPressed() != player->getKeysPressedPreviously())
         || (skipCount >=  getGame()->getPositionSkipCount())
      )
      {
         emit playerMove(player, assignedXPos, assignedYPos, directions);
      }
      else
      {
         player->setPositionSkipCounter(skipCount + 1);
      }

      player->setX(assignedXPos);
      player->setY(assignedYPos);

      // notify kick animations about player positions
      if (!player->isKilled())
      {
         emit playerPositionChanged(
            player->getId(),
            player->getX(),
            player->getY()
         );
      }
   }
   else
   {
      emit playerIdle(directions, player);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param player affected player
   \param directions directions to follow
   \param desiredXPos next desired x position
   \param desiredYPos next desired y position
*/
void CollisionDetection::updatePlayerDirections(
   Player* player,
   int keysPressed,
   qint8& directions,
   float& desiredXPos,
   float& desiredYPos
)
{
   float speed = player->getSpeed();

   if (keysPressed & Constants::KeyUp)
   {
      desiredYPos = player->getY() - (SERVER_SPEED * speed);
      directions |= Constants::KeyUp;
   }

   if (keysPressed & Constants::KeyDown)
   {
      desiredYPos = player->getY() + (SERVER_SPEED * speed);
      directions |= Constants::KeyDown;
   }

   if (keysPressed & Constants::KeyLeft)
   {
      desiredXPos = player->getX() - (SERVER_SPEED * speed);
      directions |= Constants::KeyLeft;
   }

   if (keysPressed & Constants::KeyRight)
   {
      desiredXPos = player->getX() + (SERVER_SPEED * speed);
      directions |= Constants::KeyRight;
   }
}



//-----------------------------------------------------------------------------
/*!
   \param player affected player
   \param xInField x position in the current field
   \param desiredXPos desired x position
   \param playerMovesHorizontally \c true if player moves horizontally
   \param horizontalMovementForbidden \c true if horizontal movement is forbidden
*/
float CollisionDetection::adjustXPosition(
   Player* player,
   float xInField,
   float desiredXPos,
   bool playerMovesHorizontally,
   bool horizontalMovementForbidden
)
{
   float assignedXPos = 0.0;
   float speed = player->getSpeed();

   if (
         xInField > 0.5
      && (
            horizontalMovementForbidden
         || !playerMovesHorizontally
      )
   )
   {
      if (xInField - 0.5 < (SERVER_SPEED * speed))
         assignedXPos = floor(desiredXPos) + 0.5;
      else
         assignedXPos = desiredXPos - (SERVER_SPEED * speed);
   }

   // x position is below the movement-path
   else if (
         xInField < 0.5
      && (
            horizontalMovementForbidden
         || !playerMovesHorizontally
      )
   )
   {
      if (0.5 - xInField < (SERVER_SPEED * speed))
         assignedXPos = floor(desiredXPos) + 0.5;
      else
         assignedXPos = desiredXPos + (SERVER_SPEED * speed);
   }

   // x position is on the movement path
   else
   {
      assignedXPos = desiredXPos;
   }

   return assignedXPos;
}


//-----------------------------------------------------------------------------
/*!
   \param player affected player
   \param yInField y position in the current field
   \param desiredYPos desired y position
   \param playerMovesVertically \c true if player moves vertically
   \param verticalMovementForbidden \c true if vertical movement is forbidden
*/
float CollisionDetection::adjustYPosition(
   Player* player,
   float yInField,
   float desiredYPos,
   bool playerMovesVertically,
   bool verticalMovementForbidden
)
{
   float assignedYPos = 0.0;
   float speed = player->getSpeed();

   if (
         yInField > 0.5
      && (
            verticalMovementForbidden
         || !playerMovesVertically
      )
   )
   {
      if (yInField - 0.5 < (SERVER_SPEED * speed))
         assignedYPos = floor(desiredYPos) + 0.5;
      else
         assignedYPos = desiredYPos - (SERVER_SPEED * speed);
   }

   // y position is below the movement-path
   else if (
         yInField < 0.5
      && (
            verticalMovementForbidden
         || !playerMovesVertically
      )
   )
   {
      if (0.5 - yInField < (SERVER_SPEED * speed))
         assignedYPos = floor(desiredYPos) + 0.5;
      else
         assignedYPos = desiredYPos + (SERVER_SPEED * speed);
   }

   // y position is on movement path
   else
   {
      assignedYPos = desiredYPos;
   }

   return assignedYPos;
}



//-----------------------------------------------------------------------------
/*!
   \param x x position
   \param y y position
   \param keysPressed keys pressed
   \return true is position is blocked
*/
bool CollisionDetection::isPositionBlocked(
   float x,
   float y,
   int keysPressed,
   bool verticalCheck,
   int fieldX,
   int fieldY,
   MapItem** blockingItem
)
{
   bool blocked = false;

   // add player bounds to desired positions
   if (verticalCheck)
   {
      if (keysPressed & Constants::KeyUp)
         y -= 0.5;

      if (keysPressed & Constants::KeyDown)
         y += 0.5;
   }
   else
   {
      if (keysPressed & Constants::KeyLeft)
         x -= 0.5;

      if (keysPressed & Constants::KeyRight)
         x += 0.5;
   }

   // make field position of x and y
   int fieldXPos = floor(x);
   int fieldYPos = floor(y);

   // block if map bounds are exceeded
   if (  fieldXPos < 0
      || fieldYPos < 0
      || fieldXPos > getMap()->getWidth() - 1
      || fieldYPos > getMap()->getHeight() - 1
   )
   {
      blocked = true;
   }

   // block if player hit block or stone
   else
   {
      MapItem* item = getMap()->getItem(fieldXPos, fieldYPos);

      if (
            item
         && item->isBlocking()
         && !(
               item->getX() == fieldX
            && item->getY() == fieldY
         )
      )
      {
         if (blockingItem)
         {
            *blockingItem = item;
         }

         blocked = true;
      }
   }

   return blocked;
}


//-----------------------------------------------------------------------------
/*!
   \return true if a rotation update was required
*/
bool CollisionDetection::updateRotation(
   Player* player,
   int keysPressed,
   bool moved,
   float assignedXPos,
   float assignedYPos
)
{
   bool rotationChanged = false;
   bool xMoved = false;
   bool yMoved = false;

   PlayerRotation* rotation = player->getPlayerRotation();
   float previousRotationAngle = rotation->getAngle();

   // init the new target vector depending either
   // on the player's keyboard inputs or - if no movement was allowed -
   // the previous target vector directions
   QVector2D direction;

   bool cursorKeysPressed =
         (keysPressed & Constants::KeyUp)
      || (keysPressed & Constants::KeyDown)
      || (keysPressed & Constants::KeyLeft)
      || (keysPressed & Constants::KeyRight);

   if (keysPressed & Constants::KeyUp)
      direction.setY(1.0f);

   if (keysPressed & Constants::KeyDown)
      direction.setY(-1.0f);

   if (keysPressed & Constants::KeyLeft)
      direction.setX(-1.0f);

   if (keysPressed & Constants::KeyRight)
      direction.setX(1.0f);

   // only allow 90 degree rotations
   if (moved)
   {
      xMoved = assignedXPos != player->getX();
      yMoved = assignedYPos != player->getY();

      if (!xMoved)
         direction.setX(0.0f);

      if (!yMoved)
         direction.setY(0.0f);
   }

   // set new target vector and update the according angle
   if (cursorKeysPressed || xMoved || yMoved)
      player->getPlayerRotation()->setTargetVector(direction);

   player->getPlayerRotation()->updateAngle();

   rotationChanged =
      !qFuzzyCompare(
         previousRotationAngle,
         rotation->getAngle()
      );

   return rotationChanged;
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
   \param y y position
   \param keysPressed keys pressed
   \return true is position is blocked
*/
bool CollisionDetection::isFieldBlocked(
   int *field,
   MapItem** blockingItem
)
{
   int x = field[0];
   int y = field[1];

   bool blocked = false;

   // block if map bounds are exceeded
   if (  x < 0
      || y < 0
      || x > getMap()->getWidth() - 1
      || y > getMap()->getHeight() - 1
   )
   {
      blocked = true;
   }

   // block if player hit block or stone
   else
   {
      MapItem* item = getMap()->getItem(x, y);

      if (
            item
         && item->isBlocking()
      )
      {
         if (blockingItem)
         {
            *blockingItem = item;
         }

         blocked = true;
      }
   }

   return blocked;
}


Game *CollisionDetection::getGame() const
{
   return mGame;
}


void CollisionDetection::setGame(Game *game)
{
   mGame = game;
}


Map *CollisionDetection::getMap() const
{
   return getGame()->getMap();
}

