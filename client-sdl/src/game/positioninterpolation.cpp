#include "positioninterpolation.h"

// cmath
#include <math.h>

// game
#include "bombermanclient.h"
#include "gamestatemachine.h"
#include "mapitemanimation.h"

// shared
#include "mapitem.h"
#include "framework/globaltime.h"

#include <algorithm>

#define MAP_ITEM_MOVE_EPSILON 0.05f


PositionInterpolation::PositionInterpolation(QObject *parent) :
   QObject(parent)
{
   mTimer.timeoutSignal.connect([this]() { update(); });

   connect(
      GameStateMachine::getInstance(),
      SIGNAL(stateChanged()),
      this,
      SLOT(gameStateChanged())
   );
}


void PositionInterpolation::update()
{
   float curTime= GlobalTime::Instance()->getTime();

   float clientDelta= curTime - mTime;

   // rescale client frame delta to server heartbeat interval
   float serverDelta= 1.0f / SERVER_HEARTBEAT_IN_HZ;
   float delta= clientDelta / serverDelta;

   interpolatePlayerPositions(delta);
   interpolateMapItemPositions(delta);

   mTime= curTime;
}


void PositionInterpolation::moveMapItem(
   MapItem * item,
   Constants::Direction dir,
   float speed,
   int nominalX,
   int nominalY
)
{
   // animation stopped
   if (dir == Constants::DirectionUnknown)
   {
      auto iter = mMapItemAnimations.find(item);

      // relocate bomb map item at its nominal position
      if (iter != mMapItemAnimations.end())
      {
         MapItemAnimation* anim = iter->second;
         anim->mNominalX = nominalX;
         anim->mNominalY = nominalY;
         anim->mDirection = dir;
      }
   }
   else
   {
      MapItemAnimation* animation = 0;

      if (std::find(mMapItems.begin(), mMapItems.end(), item) != mMapItems.end())
      {
         animation = mMapItemAnimations[item];
         animation->reset();
         animation->mDirection = dir;
         animation->mSpeed = speed;
      }
      else
      {
         animation = new MapItemAnimation(dir, speed);

         // pass bounce signals
         connect(
            animation,
            SIGNAL(bounce()),
            this,
            SIGNAL(bounce())
         );

         // store item data
         mMapItems.push_back(item);
         mMapItemAnimations[item] = animation;
      }
   }
}


void PositionInterpolation::removeMapItem(MapItem* item)
{
   auto it = std::find(mMapItems.begin(), mMapItems.end(), item);
   if (it != mMapItems.end())
      mMapItems.erase(it);

   auto animIt = mMapItemAnimations.find(item);
   if (animIt != mMapItemAnimations.end())
   {
      delete animIt->second;
      mMapItemAnimations.erase(animIt);
   }
}


void PositionInterpolation::interpolatePlayerPositions(float delta)
{
   const auto* players =
      BombermanClient::getInstance()->getPlayerInfoMap();

   float x = 0.0f;
   float y = 0.0f;
   float r = 0.0f;

   float dx = 0.0f;
   float dy = 0.0f;
   float dr = 0.0f;

   for (const auto& [id, player] : *players)
   {
      x = player->getX();
      y = player->getY();
      r = player->getAngle();

      dx = player->getDeltaX();
      dy = player->getDeltaY();
      dr = player->getAngleDelta();

//      qDebug("interpolation %d (%f): %f, %f, %f ", dx,dy,dr);

      x+= dx*delta;
      y+= dy*delta;
      r+= dr*delta;

      player->setPosition(
         x,
         y,
         r
      );

      emit setPlayerPosition(
         player->getId(),
         x,
         y,
         r
      );

      emit setPlayerSpeed(
         player->getId(),
         dx,
         dy,
         dr
      );
   }
}


void PositionInterpolation::interpolateMapItemPositions(float dt)
{
   foreach(MapItem* item, mMapItems)
   {
      MapItemAnimation* animation = mMapItemAnimations[item];

      animation->animate(dt);

      switch (animation->mDirection)
      {
         case Constants::DirectionUp:
            animation->mY -= animation->mSpeed*dt;
            break;
         case Constants::DirectionDown:
            animation->mY += animation->mSpeed*dt;
            break;
         case Constants::DirectionLeft:
            animation->mX -= animation->mSpeed*dt;
            break;
         case Constants::DirectionRight:
            animation->mX += animation->mSpeed*dt;
            break;
         default:
            break;
      }

      float x = item->getX() + animation->mX;
      float y = item->getY() + animation->mY;
      float z = animation->mZ;

      if (animation->mDirection == Constants::DirectionUnknown)
      {
         float diffX = x - animation->mNominalX;
         float diffY = y - animation->mNominalY;

         if (
               (fabs(diffX) > MAP_ITEM_MOVE_EPSILON)
            || (fabs(diffY) > MAP_ITEM_MOVE_EPSILON)
         )
         {
            animation->mX -= 0.1 * diffX;
            animation->mY -= 0.1 * diffY;

            // stop bouncing
            float factor = animation->mFactor;
            factor -= 0.1f;
            animation->mFactor = qMax(factor, 0.0f);

//            qDebug(
//               "anim: %f, %f, diff: %f, %f, pos: %f, %f",
//               animation->mX,
//               animation->mY,
//               diffX,
//               diffY,
//               x,
//               y
//            );
         }
      }

      emit setMapItemPosition(
         item,
         x,
         y,
         z
      );
   }
}


void PositionInterpolation::gameStateChanged()
{
   Constants::GameState state = GameStateMachine::getInstance()->getState();

   switch (state)
   {
      case Constants::GamePreparing:
      {
         qDebug("PositionInterpolation::gameStateChanged(): GameActive");
         mTimer.start(1);
         break;
      }

      case Constants::GameStopped:
      {
         qDebug("PositionInterpolation::gameStateChanged(): GameStopped");

         // stop timer
         mTimer.stop();

         // clear all animated mapitems
         mMapItems.clear();
         for (const auto& [item, anim] : mMapItemAnimations)
            delete anim;
         mMapItemAnimations.clear();

         break;
      }

      default:
         break;
   }
}

