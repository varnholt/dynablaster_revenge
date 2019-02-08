#pragma once

#include "math/vector2.h"
#include "math/matrix.h"
#include <QMap>

class Camera;
class PlayerInfo;

class CameraInterpolation
{
public:
   CameraInterpolation(Camera* center, Camera* upper, Camera* lower);

   void resetPlayerPositions();
   void startPositionUpdate(float width, float height, float dt);
   void addPlayerPosition( PlayerInfo* player );
   void endPlayerPositionUpdate();
   Matrix getCameraMatrix(float time, float scale = 1.0f);
   bool isPlayerMapEmpty() const;

private:
   float mInvWidth;
   float mInvHeight;
   Camera* mCenter;
   Camera* mUpper;
   Camera* mLower;
   Vector2 mCurrPos;
   Vector2 mPrevPos[3];
   Vector2 mInterPos[3];
   float   mTime;
   float   mDeltaTime;
   int     mActivePlayers;
   QMap<PlayerInfo*, float> mPlayers;
};

