#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

// Qt
#include <QObject>

// shared
#include "constants.h"


// forward declarations
class Game;
class Map;
class MapItem;
class Player;

class CollisionDetection : public QObject
{
   Q_OBJECT

public:

   //! constructor
   CollisionDetection(QObject *parent = 0);

   //! process player
   void process(Player* player);
   
   //! returns true if position is blocked
   bool isFieldBlocked(
      int* field,
      MapItem** blockingItem = 0
   );

   //! getter for game
   Game* getGame() const;


signals:

   //! player kicks a bomb
   void playerKicksBomb(
      Player* player,
      MapItem* item,
      bool verticallyKicked,
      int keysPressed
   );
   
   //! send an idle packet
   void playerIdle(qint8 directions, Player* player);

   //! move player
   void playerMove(
      Player* player,
      float assignedXPos,
      float assignedYPos,
      qint8 directions
   );

   //! player position was updated
   void playerPositionChanged(int id, float x, float y);


public slots:

   //! setter for game
   void setGame(Game* game);


protected:

   //! getter for game
   Map* getMap() const;

   //! prepare referenced values for current player
   void updatePlayerDirections(
      Player* player,
      int keysPressed,
      qint8& directions,
      float& desiredXPos,
      float& desiredYPos
   );

   //! adjust player x position
   float adjustXPosition(
      Player* player,
      float xInField,
      float desiredXPos,
      bool playerMovesHorizontally,
      bool horizontalMovementForbidden
   );

   //! adjust player y position
   float adjustYPosition(
      Player* player,
      float yInField,
      float desiredYPos,
      bool playerMovesVertically,
      bool verticalMovementForbidden
   );

   //! update the player's rotation
   bool updateRotation(
      Player* player,
      int keysPressed,
      bool moved,
      float assignedXPos,
      float assignedYPos
   );

   //! returns true if position is blocked
   bool isPositionBlocked(
      float x,
      float y,
      int keysPressed,
      bool verticalCheck,
      int fieldX,
      int fieldY,
      MapItem** blockingItem = 0
   );

   //! game
   Game* mGame;
};

#endif // COLLISIONDETECTION_H
