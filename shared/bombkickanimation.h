#ifndef BOMBKICKANIMATION_H
#define BOMBKICKANIMATION_H

// Qt
#include <QMap>
#include <QObject>
#include <QPoint>

// shared
#include "constants.h"

// forward declarations
class BombMapItem;
class Map;
class MapItem;
class QTimer;


class BombKickAnimation : public QObject
{
   Q_OBJECT

   public:

      //! constructor
      BombKickAnimation(QObject *parent = 0);

      //! destructor
      virtual ~BombKickAnimation();

      //! remove all animations
      static void deleteAll();

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


   signals:

      //! bomb may explode now
      void explode();

      //! animation started
      void started(
         Constants::Direction direction,
         float speed
      );

      //! animation stopped
      void stopped();


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
      QTimer* mTimer;

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
      QMap<int, QPoint> mPlayerPositions;


      // inter-bomb-collisions

      //! list of kick animations
      static QList<BombKickAnimation*> sAnimations;

      //! bomb is colliding with another bomb
      bool mColliding;
};

#endif // BOMBKICKANIMATION_H


