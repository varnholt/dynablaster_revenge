/***************************************************************************
                                 extraanimation
 ---------------------------------------------------------------------------
    project              : dynablaster revenge client
    begin                : january 2013
    copyright            : (C) 2013 by mueslee
    author               : mueslee
    email                : nope
***************************************************************************/

// header
#include "extraanimation.h"

// Qt
#include <QGLContext>
#include <QImage>

#include "tools/random.h"

// framework
#include <math.h>

// static
int ExtraAnimation::sPointspriteTexture = -1;
int ExtraAnimation::sHalfSphereTexture = -1;
int ExtraAnimation::sFullSphereTexture = -1;
int ExtraAnimation::sFullSphereTextureRed = -1;

#define TIME_EXPAND 500.0f
#define TIME_FADE 1000.0f
#define TIME_POINTSPRITE_DESTRUCT 100.0f
#define SPHERE_SCALE_HALF 3.5f
#define SPHERE_SCALE_FULL 3.5f
#define POINTSPRITE_SCALE 0.25f
#define POINTSPRITE_LAYERS 5
#define POINTSPRITE_SPRITES_PER_LAYER 10


//-----------------------------------------------------------------------------
/*!
*/
ExtraAnimation::ExtraAnimation()
  : mExtraDestroyed(false)
{
}


//-----------------------------------------------------------------------------
/*!
*/
void ExtraAnimation::initializeTextures()
{
   QImage pointsprite = QImage("data/game/pointsprite.png");

   sPointspriteTexture =
      ((QGLContext*)QGLContext::currentContext())->bindTexture(
         pointsprite,
         GL_TEXTURE_2D,
         GL_RGBA,
         QGLContext::LinearFilteringBindOption
      );

   QImage sphereHalf = QImage("data/game/extrasphere_half.png");

   sHalfSphereTexture =
      ((QGLContext*)QGLContext::currentContext())->bindTexture(
         sphereHalf,
         GL_TEXTURE_2D,
         GL_RGBA,
         QGLContext::LinearFilteringBindOption
      );

   QImage sphereFull = QImage("data/game/extrasphere_full.png");

   sFullSphereTexture =
      ((QGLContext*)QGLContext::currentContext())->bindTexture(
         sphereFull,
         GL_TEXTURE_2D,
         GL_RGBA,
         QGLContext::LinearFilteringBindOption
      );

   QImage sphereFullRed = QImage("data/game/extrasphere_full_red.png");

   sFullSphereTextureRed =
      ((QGLContext*)QGLContext::currentContext())->bindTexture(
         sphereFullRed,
         GL_TEXTURE_2D,
         GL_RGBA,
         QGLContext::LinearFilteringBindOption
      );

}

//-----------------------------------------------------------------------------
/*!
*/
void ExtraAnimation::initPointsprites()
{
    // init
    mAnimationTime.start();
    mSprites = new ExtraPointSprite*[POINTSPRITE_LAYERS * POINTSPRITE_SPRITES_PER_LAYER];

    float xPos = 0.0f;
    float yPos = 0.0f;
    float zPos = 0.0f;

    Vector origin = getOrigin();


    /*
       build n layers of pointsprites

       ^
       +---------------------+ 1, 1
     0 | x x x x x x x x x x |
     1 |  x x x x x x x x x  |
     2 | x x x x x x x x x x |
     3 |  x x x x x x x x x  |
     4 | x x x x x x x x x x |
     n |  x x x x x x x x x  |
       +---------------------+-->
     0, 0
    */

    for (int z = 0; z < POINTSPRITE_LAYERS; z++)
    {
       zPos = 1.0 - (z / (float)POINTSPRITE_LAYERS);

       for (int x = 0; x < POINTSPRITE_SPRITES_PER_LAYER; x++)
       {
          xPos = (x / (float)POINTSPRITE_SPRITES_PER_LAYER) - 0.5f;

          // init point sprite
          ExtraPointSprite* sprite = new ExtraPointSprite();
          mSprites[z * POINTSPRITE_SPRITES_PER_LAYER + x] = sprite;

          sprite->mPosition.x = xPos + origin.x;
          sprite->mPosition.y = yPos + origin.y;
          sprite->mPosition.z = zPos + origin.z;

          sprite->mOrigin = origin;

          // init sprite direction
          float dirRandX = 0.0f;
          float dirRandY = 0.0f;
          float dirRandZ = 0.0f;

          // -0.25 .. 0.25
          dirRandX = frands(-0.75f, 0.75f) * 3.0f;
          dirRandY = frands(-0.75f, 0.75f) * 3.0f;
          dirRandZ = frands(-0.75f, 0.75f) * 3.0f;

          sprite->mDirection =
             Vector(
                dirRandX,
                dirRandY,
                dirRandZ
             );
       }
    }
}


void ExtraAnimation::initSphere()
{
}


void ExtraAnimation::initialize()
{
   if (sPointspriteTexture < 0)
      initializeTextures();

   initPointsprites();
   initSphere();
}


//-----------------------------------------------------------------------------
/*!
*/
void ExtraAnimation::updateSpritePosition(
   float dt,
   ExtraPointSprite* sprite
)
{
   sprite->mScalar    += dt * 0.025f;

   // make pointsprites smaller
   sprite->mIntensity -= dt * 0.04f;
   sprite->mIntensity = qMax(0.0f, sprite->mIntensity);

   // let them go up faster
   sprite->mDirection.z = sprite->mDirection.z + dt * 0.03f;

   sprite->mPosition =
         sprite->mOrigin
      + (sprite->mDirection * sprite->mScalar);

   /*
   qDebug(
      "origin: %f, %f, %f; pos: %f, %f, %f; "
      "dir: %f, %f, %f; intensity: %f, scalar: %f",
      sprite->mOrigin.x,
      sprite->mOrigin.y,
      sprite->mOrigin.z,
      sprite->mPosition.x,
      sprite->mPosition.y,
      sprite->mPosition.z,
      sprite->mDirection.x,
      sprite->mDirection.y,
      sprite->mDirection.z,
      sprite->mIntensity,
      sprite->mScalar
   );
   */
}


void ExtraAnimation::setOrigin(const Vector &origin)
{
   mOrigin = origin;
}


const Vector &ExtraAnimation::getOrigin() const
{
   return mOrigin;
}


bool ExtraAnimation::isElapsed() const
{
   return mAnimationTime.elapsed() > TIME_EXPAND;
}


void ExtraAnimation::setExtraDestroyed(bool destroyed)
{
   mExtraDestroyed = destroyed;
}


bool ExtraAnimation::isExtraDestroyed() const
{
   return mExtraDestroyed;
}


void ExtraAnimation::drawPointSprites(float dt)
{
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);

   glEnable(GL_TEXTURE_2D);

   // bind point sprite texture
   glBindTexture(GL_TEXTURE_2D, sPointspriteTexture);

   float pointSize = 0.0f;

   ExtraPointSprite* sprite = 0;

   Vector pos;

   int yMax =
      qMin(
         (int)((mAnimationTime.elapsed() / TIME_POINTSPRITE_DESTRUCT) * POINTSPRITE_LAYERS),
         POINTSPRITE_LAYERS
      );

   for (int z = 0; z < yMax; z++)
   {
      for (int x = 0; x < POINTSPRITE_SPRITES_PER_LAYER; x ++)
      {
         // update sprite
         sprite = getSprite(x, z);
         updateSpritePosition(dt, sprite);
      }
   }

   float normalizedTime = qMin((mAnimationTime.elapsed() / TIME_FADE), 1.0f);
   float alpha = 1.0f - normalizedTime;

   // draw it
   glBegin(GL_QUADS);

   glColor4f(
      1.0f,
      1.0f,
      1.0f,
      alpha
   );

   for (int z = 0; z < POINTSPRITE_LAYERS; z++)
   {
      for (int x = 0; x < POINTSPRITE_SPRITES_PER_LAYER; x ++)
      {
         sprite = getSprite(x, z);

         pos = sprite->mPosition;
         pointSize = sprite->mSize * sprite->mIntensity * POINTSPRITE_SCALE;

         glTexCoord2f(0.0f, 0.0f);
         glVertex3f(
            pos.x - pointSize,
            pos.y,
            pos.z - pointSize
         );

         glTexCoord2f(1.0f, 0.0f);
         glVertex3f(
            pos.x + pointSize,
            pos.y,
            pos.z - pointSize
         );

         glTexCoord2f(1.0f, 1.0f);
         glVertex3f(
            pos.x + pointSize,
            pos.y,
            pos.z + pointSize
         );

         glTexCoord2f(0.0f, 1.0f);
         glVertex3f(
            pos.x - pointSize,
            pos.y,
            pos.z + pointSize
         );
      }
   }
   glEnd();

   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void ExtraAnimation::drawHalfSphere(float dt)
{
   Q_UNUSED(dt);
   glEnable(GL_DEPTH_TEST); // use reading z buffer
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);

   // bind point sprite texture
   glBindTexture(GL_TEXTURE_2D, sHalfSphereTexture);

   float normalizedTime = qMin( (mAnimationTime.elapsed() / TIME_EXPAND), 1.0f);
   float quadSize = normalizedTime * SPHERE_SCALE_HALF;
   float alpha = 1.0f - normalizedTime;

   // qDebug("ntime: %f, quads: %f, alpha: %f", normalizedTime, quadSize, alpha);

   Vector pos = getOrigin();

   glColor4f(
       1.0f,
       1.0f,
       1.0f,
       alpha
   );

   int sphereCount = 1;

   for (int i = 0; i < sphereCount; i++)
   {
       // draw it
       glBegin(GL_QUADS);

       float xScale = 0.50f * quadSize;

       glTexCoord2f(0.0f, 1.0f);
       glVertex3f(
           pos.x - xScale,
           pos.y,
           pos.z
       );

       glTexCoord2f(1.0f, 1.0f);
       glVertex3f(
           pos.x + xScale,
           pos.y,
           pos.z
       );

       glTexCoord2f(1.0f, 0.0f);
       glVertex3f(
           pos.x + xScale,
           pos.y,
           pos.z + quadSize
       );

       glTexCoord2f(0.0f, 0.0f);
       glVertex3f(
           pos.x - xScale,
           pos.y,
           pos.z + quadSize
       );

       glEnd();
   }

   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void ExtraAnimation::drawFullSphere(float dt)
{
   Q_UNUSED(dt);

   glEnable(GL_DEPTH_TEST); // use reading z buffer
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   glEnable(GL_TEXTURE_2D);

   // bind point sprite texture
   glBindTexture(
      GL_TEXTURE_2D,
      isExtraDestroyed()
         ? sFullSphereTextureRed
         : sFullSphereTexture
   );

   float normalizedTime = qMin( (mAnimationTime.elapsed() / TIME_EXPAND), 1.0f);
   float quadSize = normalizedTime * SPHERE_SCALE_FULL;
   float alpha = 1.0f - normalizedTime;

   float zIncrement = quadSize * 0.4f;

   Vector pos = getOrigin();

   glColor4f(
       1.0f,
       1.0f,
       1.0f,
       alpha
   );

   // draw it
   glBegin(GL_QUADS);

   float scale = 0.5f * quadSize;

   glTexCoord2f(0.0f, 1.0f);
   glVertex3f(
       pos.x - scale,
       pos.y + scale,
       pos.z + zIncrement
   );

   glTexCoord2f(0.0f, 0.0f);
   glVertex3f(
       pos.x - scale,
       pos.y - scale,
       pos.z + zIncrement
   );

   glTexCoord2f(1.0f, 0.0f);
   glVertex3f(
       pos.x + scale,
       pos.y - scale,
       pos.z + zIncrement
   );

   glTexCoord2f(1.0f, 1.0f);
   glVertex3f(
       pos.x + scale,
       pos.y + scale,
       pos.z + zIncrement
   );

   glEnd();

   // disable the most annoying stuff
   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glColor4f(1,1,1,1);
}


void ExtraAnimation::draw(float dt)
{
   // init blending
   if (!isExtraDestroyed())
      drawPointSprites(dt);

   drawFullSphere(dt);
}


ExtraAnimation::ExtraPointSprite *ExtraAnimation::getSprite(
   int x,
   int y
)
{
   return mSprites[y * POINTSPRITE_SPRITES_PER_LAYER + x];
}




