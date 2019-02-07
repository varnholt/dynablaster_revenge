#include "camerainterpolation.h"
#include "framework/globaltime.h"
#include "playerinfo.h"
#include "nodes/camera.h"
#include "tools/random.h"
#include <math.h>

CameraInterpolation::CameraInterpolation(Camera* center, Camera* upper, Camera* lower)
   : mInvWidth(0.0f)
   , mInvHeight(0.0f)
   , mCenter( center )
   , mUpper( upper )
   , mLower( lower )
   , mCurrPos( 0.0f, 0.0f )
   , mTime( 0.0f )
{
   for (int i=0;i<3;i++)
   {
      mPrevPos[i].set(0.5f, 0.5f);
      mInterPos[i].set(0.5f, 0.5f);
   }

   mTime= GlobalTime::Instance()->getTime();
}

void CameraInterpolation::resetPlayerPositions()
{
   mPlayers.clear();
   mActivePlayers= 0;
}

void CameraInterpolation::startPositionUpdate(float width, float height, float dt)
{
   mInvWidth= 1.0f / width;
   mInvHeight= 1.0f / height;
   mDeltaTime= dt*0.25f;
   mActivePlayers= 0;
   mCurrPos.set( 0.0f, 0.0f );
}

void CameraInterpolation::addPlayerPosition( PlayerInfo* player )
{
   float fade= 1.0f;

   QMap<PlayerInfo*, float>::Iterator it;
   it= mPlayers.find( player );
   if (it != mPlayers.end())
   {
      if (player->isKilled())
      {
         fade= it.value();
         fade-=mDeltaTime;
         if (fade < 0.0f)
            fade= 0.0f;
         *it= fade;
      }
   }
   else
   {
      mPlayers.insert(player, 1.0f);
   }

   if (fade > 0.0f)
   {
      float x= player->getX() * mInvWidth;
      float y= player->getY() * mInvHeight;
      mCurrPos += Vector2(x,y) * fade;
      mActivePlayers++;
   }
}

void CameraInterpolation::endPlayerPositionUpdate()
{
   const float timeStep= 1.0f / 60.0f; // update positions 30x per second
   float time= GlobalTime::Instance()->getTime();

   if (mActivePlayers > 0)
   {
      mCurrPos = mCurrPos * (1.0f / mActivePlayers);
   }
   else
   {
      mCurrPos.set( 0.5f, 0.5f );
   }

   while (time >= mTime+timeStep)
   {
      // randomize
      float dx= fabs(mCurrPos.x - mInterPos[0].x);
      float dy= fabs(mCurrPos.y - mInterPos[0].y);
      if (dx<1.0f) dx=1.0f;
      if (dy<1.0f) dy=1.0f;

      mPrevPos[2]= mPrevPos[1];
      mPrevPos[1]= mPrevPos[0];
      mPrevPos[0]= mCurrPos;// + Vector2( frands( -0.1f, 0.1f ) / dx, frands( -0.1f, 0.1f ) / dy );

      Vector2 newPos=
           mPrevPos[0] * 0.00105143353171677460f
         + mPrevPos[1] * 0.00210286706343354920f
         + mPrevPos[2] * 0.00105143353171677460f
//         + mInterPos[0] * -1.0f
         + mInterPos[0] * 1.92916981735421360000f
         + mInterPos[1] * -0.93337555158934948000f
      ;

/*
      x[0] = NewSample;
      y[0] = ACoef[0] * x[0];
      for(n=1; n<=NCoef; n++)
          y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];
*/

      mInterPos[2]= mInterPos[1];
      mInterPos[1]= mInterPos[0];
      mInterPos[0]= newPos;

      mTime += timeStep;
   }
}

Matrix CameraInterpolation::getCameraMatrix(float time)
{
   float fov= 1.0f;
   Matrix cam;

   if (mCenter)
   {
      mCenter->transform( time );
      fov= 1.0f / ((float)tan( mCenter->getFOV() * 0.5 ) * 0.75f);
      cam= mCenter->getTransform();
   }

   if (mUpper && mLower)
   {
      mUpper->transform( mInterPos[0].x * 16000.0f ); // 100 frame a 160 ticks
      mLower->transform( mInterPos[0].x * 16000.0f );


      Matrix upper= mUpper->getTransform();
      Matrix lower= mLower->getTransform();

      Matrix ip= Matrix::blend(upper, lower, mInterPos[0].y);

      if (time < 60*160)
      {
         // no influence of player positions
      }
      else
      if (time < 130*160)
      {
         float t= (time-60*160) / (70*160);
         t= 0.5f - (float)cos(t*M_PI) * 0.5f;
         t= t*t;
         cam= Matrix::blend(cam, ip, t);
      }
      else
      {
         cam= ip;
      }
   }

   cam= cam.getView() * Matrix::scale(fov, fov, 1.0f);

   return cam;
}


bool CameraInterpolation::isPlayerMapEmpty() const
{
   return (mActivePlayers == 0);
}

