#ifndef BOMBKICKANIMATION_H
#define BOMBKICKANIMATION_H

// Qt
#include <QObject>

// shared
#include "constants.h"
#include "point.h"
#include "signal.h"
#include "timer.h"

#include <functional>
#include <unordered_map>
#include <vector>

// forward declarations
class BombMapItem;
class Map;
class MapItem;

class BombKickAnimation : public QObject
{
   Q_OBJECT

public:
   //! constructor
   BombKickAnimation(QObject* parent = 0);

   //! destructor
   virtual ~BombKickAnimation();

   //! remove all animations
   static void deleteAll();

   //! run an arbitrary callback right before this animation is destroyed - used by whoever
   //! subscribed a Signal<> connected against a longer-lived signal (Game's own signals live
   //! for the whole match, this animation doesn't) to disconnect itself, since Signal<> has
   //! no automatic disconnect-on-destroy the way Qt's own connect() did
   void addDestroyCallback(std::function<void()> callback);

   //! start animation
   void start();

   //! setter for map
   void setMap(Map* map);

   //! setter for x position
   void setX(float x);

   //! setter for y position
   void setY(float y);

   //! getter for x position
   float getX() const;

   //! getter for y position
   float getY() const;

   //! getter for map
   Map* getMap() const;

   //! check if ready to explode
   bool isReadyToExplode() const;

   //! set bomb to "ready" to explode
   void setReadyToExplode(bool ready);

   //! setter for direction
   void setDirection(Constants::Direction dir);

   //! ignite animation at x, y
   static void ignite(int x, int y);

public:
   // Signal<> replacements for BombKickAnimation's former Qt signals (see
   // project_full_qt_removal_scope memory).

   //! bomb may explode now
   Signal<> explodeSignal;

   //! animation started
   Signal<Constants::Direction, float> startedSignal;

   //! animation stopped
   Signal<> stoppedSignal;

public slots:

   //! enable mReadyToExplode flag
   void readyToExplode();

   //! update a player position
   void updatePlayerPosition(int id, float x, float y);

   //! remove player position if player died
   void removePlayerPosition(int id);

protected slots:

   //! update the bomb's position
   void updatePosition();

protected:
   //! check if values are in range
   bool isInRange(float value1, float value2, float epsilon);

   //! movement may be continued
   bool isMoveAllowed();

   //! getter for step speed
   float getStepSize() const;

   //! getter for direction
   Constants::Direction getDirection() const;

   //! getter for x direction
   int getDirectionX() const;

   //! getter for y direction
   int getDirectionY() const;

   //! unmap bomb
   void unmapBomb();

   //! remap bomb
   void remapBomb();

   //! reset animation state
   void reset();

   // inter-bomb-collisions

   //! add animation to static list
   static void addAnimation(BombKickAnimation* animation);

   //! remove animation from static list
   static void removeAnimation(BombKickAnimation* animation);

   //! check if bomb collides with another bomb
   bool checkCollision(BombKickAnimation* animation);

   //! getter for colliding flag
   bool isColliding() const;

   //! setter for colliding flag
   void setColliding(bool colliding);

   //! update collisions
   void updateCollisions();

   //! animation update timer
   Timer mTimer;

   //! intensity factor
   float mFactor;

   //! irection
   Constants::Direction mDirection;

   //! x position
   float mX;

   //! y position
   float mY;

   //! ready to explode flag
   bool mReadyToExplode;

   //! ptr to game
   Map* mMap;

   //! bomb map item
   BombMapItem* mBombMapItem;

   //! player positions to collide with
   std::unordered_map<int, Point> mPlayerPositions;

   //! run in the destructor - see addDestroyCallback()
   std::vector<std::function<void()>> mDestroyCallbacks;

   // inter-bomb-collisions

   //! list of kick animations
   static std::vector<BombKickAnimation*> sAnimations;

   //! bomb is colliding with another bomb
   bool mColliding;
};

#endif  // BOMBKICKANIMATION_H
