#include "playeritem.h"
#include "math/matrix.h"
#include "nodes/mesh.h"
#include "animation/motionmixer.h"

PlayerItem::PlayerItem(int id, const QString& nick, Constants::Color color)
: mID(id)
, mColor(color)
, mNick(nick)
, mMesh(0)
, mMaterial(0)
, mPos(0,0,0)
, mRot(0)
, mSpeed(0.0f)
, mAnimBlend(0.0f)
, mStandBlend(0.0f)
, mKilled(false)
, mWin(false)
, mLeftFoot(false)
, mFlash(0.0f)
{
}

PlayerItem::~PlayerItem()
{
   if (mMesh)
      delete mMesh;
}

void PlayerItem::kill()
{
   mKilled= true;
   mMesh->setFrame( 0.0f );
}

void PlayerItem::win()
{
   mWin= true;
   mMesh->setFrame( 0.0f );
}

bool PlayerItem::isWinner() const
{
   return mWin;
}

void PlayerItem::setKilled(bool killed)
{
   mKilled= killed;
}

bool PlayerItem::isKilled() const
{
   return mKilled;
}

int PlayerItem::getID() const
{
	return mID;
}

Constants::Color PlayerItem::getColor() const
{
   return mColor;
}

const QString& PlayerItem::getNick() const
{
	return mNick;
}

void PlayerItem::setMaterial(Material *mat)
{
	mMaterial= mat;
}

Material* PlayerItem::getMaterial() const
{
	return mMaterial;
}

void PlayerItem::setMesh(Mesh *mesh)
{
	mMesh= mesh;
}

Mesh* PlayerItem::getMesh() const
{
   return mMesh;
}

void PlayerItem::setRotation(float rot)
{
	mRot= rot;
	update();
}

void PlayerItem::setPosition(float x, float y)
{
	mPos= Vector(x,-y,0);
   update();
}

const Vector &PlayerItem::getPosition() const
{
   return mPos;
}


void PlayerItem::update()
{
   Matrix pos= Matrix::position(mPos.x, mPos.y, 0.0f);
   Matrix mat= Matrix::rotateZ(mRot);
   Matrix scale= Matrix::scale(3.0f, 3.0f, 3.0f); //= Matrix::scale(0.03f, 0.03f, 0.03f);
	mMesh->setUserTransformable(true);
	mMesh->setTransform(mat * scale * pos);
}

void PlayerItem::setSpeed(float speed)
{
   mSpeed= speed;

   // MotionMixer *mixer= mMesh->getMotionMixer();
   if (speed > 0.0f)
   {
      float sp= (mSpeed-0.05) * 20.0;
      if (sp>1.0f) speed=1.0f;
      if (sp<0.0f) speed=0.0f;
      mAnimBlend= 1.0f - sp;
   }
   else
   {
//      mStandBlend= 0.0f;
   }
}


float PlayerItem::getSpeed() const
{
   return mSpeed;
}


void PlayerItem::setFlash(float flash)
{
   mFlash= flash;
}


void PlayerItem::animate(float /*time*/, float delta)
{
   if (mFlash >= delta*0.05f)
      mFlash -= delta*0.05f;
   else
      mFlash= 0.0f;

   if (mMesh)
      mMesh->setRenderParameter(0, mFlash);

   if (mSpeed > 0.0f || mKilled || mWin)
   {
      if (mStandBlend > 0.0f)
      {
         // blend from stand to actual animation pose
         mStandBlend -= delta*0.2;
//         mStandBlend = 0.0f;
         if (mStandBlend <= 0.0f)
            mStandBlend= 0.0f;
      }

      float frame= mMesh->getFrame() + (mSpeed+1.0f) * 100.0 * delta;
      if (!mKilled && !mWin)
         while (frame > 4000.0) frame -= 4000.0;
      mMesh->setFrame( frame );
   }
   else
   {
      if (mStandBlend < 1.0f)
      {
         // blend into stand pose
         mStandBlend += delta*0.1;
         if (mStandBlend >= 1.0f)
         {
            // nearest pose to "stand", start with left or right foot
            if (mLeftFoot)
               mMesh->setFrame( 833.0 );
            else
               mMesh->setFrame( 4000 - 833.0 );
            mLeftFoot= !mLeftFoot;
            mStandBlend= 1.0f;
         }
      }
   }

   MotionMixer *mixer= mMesh->getMotionMixer();
   if (mixer)
   {
      if (mKilled)
         mixer->setAnimation(3, 3, 1.0f, 3, 0.0f);
      else if (mWin)
         mixer->setAnimation(4, 4, 1.0f, 4, 0.0f);
      else
         mixer->setAnimation(1, 2, 1.0f-mAnimBlend, 0, mStandBlend);
   }
}



