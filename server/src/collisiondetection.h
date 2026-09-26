#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

// Qt
#include <QObject>

// shared
#include "constants.h"
#include "signal.h"

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
   CollisionDetection(QObject* parent = 0);

   //! process player
   void process(Player* player);

   //! returns true if position is blocked
   bool isFieldBlocked(int* field, MapItem** blockingItem = 0);

   //! getter for game
   Game* getGame() const;

public:
   // Signal<> replacements for CollisionDetection's former Qt signals (see
   // project_full_qt_removal_scope memory).

   //! player kicks a bomb
   Signal<Player*, MapItem*, bool, int> playerKicksBombSignal;

   //! send an idle packet
   Signal<int8_t, Player*> playerIdleSignal;

   //! move player
   Signal<Player*, float, float, int8_t> playerMoveSignal;

   //! player position was updated
   Signal<int, float, float> playerPositionChangedSignal;

public slots:

   //! setter for game
   void setGame(Game* game);

protected:
   //! getter for game
   Map* getMap() const;

   //! prepare referenced values for current player
   void updatePlayerDirections(Player* player, int keysPressed, int8_t& directions, float& desiredXPos, float& desiredYPos);

   //! adjust player x position
   float adjustXPosition(Player* player, float xInField, float desiredXPos, bool playerMovesHorizontally, bool horizontalMovementForbidden);

   //! adjust player y position
   float adjustYPosition(Player* player, float yInField, float desiredYPos, bool playerMovesVertically, bool verticalMovementForbidden);

   //! update the player's rotation
   bool updateRotation(Player* player, int keysPressed, bool moved, float assignedXPos, float assignedYPos);

   //! returns true if position is blocked
   bool isPositionBlocked(float x, float y, int keysPressed, bool verticalCheck, int fieldX, int fieldY, MapItem** blockingItem = 0);

   //! game
   Game* mGame;
};

#endif  // COLLISIONDETECTION_H
