// header
#include "extrarevealanimation.h"

// framework
#include "gldevice.h"

// Qt
#include <QGLContext>
#include <QImage>

// cmath
#include <math.h>

// static
int ExtraRevealAnimation::sFrustumTexture = -1;


#define GROW_TIME 50.0f
#define OPEN_TIME 50.0f
#define ANIMATION_TIME 75.0f

#define FADE_OUT_START_TIME 50.0f
#define FADE_OUT_DURATION 25.0f

#define FRUSTUM_TOP_SCALE 1.5f
#define FRUSTUM_HEIGHT 3.0f

unsigned int ExtraRevealAnimation::mShader = 0;
int ExtraRevealAnimation::sBlendColorParameter = -1;
int ExtraRevealAnimation::sTextureScrollParameter = -1;


//-----------------------------------------------------------------------------
/*!
*/
ExtraRevealAnimation::ExtraRevealAnimation()
 : mX(0),
   mY(0),
   mElapsed(0.0f)
{
}


//-----------------------------------------------------------------------------
/*!
*/
void ExtraRevealAnimation::initializeTextures()
{
   QImage sphereFull = QImage("data/game/extra_frustum.png");

   sFrustumTexture =
      ((QGLContext*)QGLContext::currentContext())->bindTexture(
         sphereFull,
         GL_TEXTURE_2D,
         GL_RGBA,
         QGLContext::LinearFilteringBindOption
      );

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


//-----------------------------------------------------------------------------
/*!
*/
void ExtraRevealAnimation::initialize()
{
   if (sFrustumTexture < 0)
      initializeTextures();

   if (sBlendColorParameter == -1)
   {
      // init shader
      GLDevice* dev = (GLDevice*)activeDevice;
      mShader = dev->loadShader("extrareveal-vert.glsl", "extrareveal-frag.glsl");
      sBlendColorParameter = dev->getParameterIndex("color");
      sTextureScrollParameter = dev->getParameterIndex("textureScroll");
   }
}


bool ExtraRevealAnimation::isElapsed() const
{
   return mElapsed > ANIMATION_TIME;
}


void ExtraRevealAnimation::setPos(int x, int y)
{
   mX = x;
   mY = y;
}


void ExtraRevealAnimation::drawFrustum(float dt)
{
   /*

     y and z are flipped

     4 sides:

         +--------------+ 1.0, 1.0, 1.0
        /              /|
       /              / |
      /              /  |
     +--------------+   |
     |              |   |
     |              |   |
     |              |   |
     |              |   + 1.0, 1.0, 0.0
     |              |  /
     |              | /
     +--------------+/
    0.0, 0.0, 0.0  1.0, 0.0, 0.0


    scale = 0.6
      => offset = (1.0 - scale) / 2

     |              |
     +--------------+

   */

   mElapsed += dt;

   float bottomScale = 0.75f;
   float bottomOffset = (1.0f - bottomScale) * 0.5f;

   float topScale = qMin(mElapsed / OPEN_TIME, 1.0f) * FRUSTUM_TOP_SCALE;
   float topOffset = (1.0f - topScale) * 0.5f;

   float b0 = bottomOffset;
   float b1 = bottomOffset + bottomScale;

   float t0 = topOffset;
   float t1 = topOffset + topScale;

   float z = 0.0f;

   float h = qMin(mElapsed / GROW_TIME, 1.0f) * FRUSTUM_HEIGHT;

   float alpha = 1.0f;

   if (mElapsed > FADE_OUT_START_TIME)
   {
      alpha = qMin(1.0f, (mElapsed - FADE_OUT_START_TIME) / FADE_OUT_DURATION);
      alpha = 0.5f * (1.0f + cos(alpha * M_PI));
   }

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);

   glDepthMask(false);
   activeDevice->setCulling(false);

   glActiveTexture(GL_TEXTURE0);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, sFrustumTexture);

   activeDevice->setShader(mShader);

   activeDevice->setParameter(
      sBlendColorParameter,
      Vector4(1.0f, 1.0f, 1.0f, alpha)
   );

   activeDevice->setParameter(
      sTextureScrollParameter,
      Vector(mElapsed, -mElapsed, 0.0f) * 0.005f
   );

   glPushMatrix();

   glTranslatef(mX, -mY - 1.0f, 0.0f);

   glBegin(GL_QUADS);

   // back
   glTexCoord2f(0.5f,  1.0f); glVertex3f(b0, b1, z);
   glTexCoord2f(0.75f, 1.0f); glVertex3f(b1, b1, z);
   glTexCoord2f(0.75f, 0.0f); glVertex3f(t1, t1, z+h);
   glTexCoord2f(0.5f,  0.0f); glVertex3f(t0, t1, z+h);

   // front
   glTexCoord2f(0.0f,  1.0f); glVertex3f(b0, b0, z);
   glTexCoord2f(0.25f, 1.0f); glVertex3f(b1, b0, z);
   glTexCoord2f(0.25f, 0.0f); glVertex3f(t1, t0, z+h);
   glTexCoord2f(0.0f,  0.0f); glVertex3f(t0, t0, z+h);

   // left
   glTexCoord2f(0.75f, 1.0f); glVertex3f(b0, b0, z);
   glTexCoord2f(1.0f,  1.0f); glVertex3f(b0, b1, z);
   glTexCoord2f(1.0f,  0.0f); glVertex3f(t0, t1, z+h);
   glTexCoord2f(0.75f, 0.0f); glVertex3f(t0, t0, z+h);

   // right
   glTexCoord2f(0.25f, 1.0f); glVertex3f(b1, b0, z);
   glTexCoord2f(0.5f,  1.0f); glVertex3f(b1, b1, z);
   glTexCoord2f(0.5f,  0.0f); glVertex3f(t1, t1, z+h);
   glTexCoord2f(0.25f, 0.0f); glVertex3f(t1, t0, z+h);

   glEnd();

   glPopMatrix();

   // disable the most annoying stuff
   glDepthMask(true);
   activeDevice->setCulling(true);
   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   activeDevice->setShader(0);
}


void ExtraRevealAnimation::draw(float dt)
{
   drawFrustum(dt);
}

