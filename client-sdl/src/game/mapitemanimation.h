#ifndef MAPITEMANIMATION_H
#define MAPITEMANIMATION_H

// Qt
#include <QObject>

// shared
#include "constants.h"


class MapItemAnimation : public QObject
{
   Q_OBJECT

   public:

      MapItemAnimation(
         Constants::Direction dir,
         float speed
      );

      void animate(float dt);
      void reset();

      Constants::Direction mDirection;
      float mSpeed;
      float mX;
      float mY;
      float mZ;
      float mZPrev;
      int mNominalX;
      int mNominalY;
      float mTime;
      bool mTick;
      float mFactor;


   signals:

      void bounce();
};

#endif // MAPITEMANIMATION_H
