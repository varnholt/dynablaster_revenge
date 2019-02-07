// header
#include "ribbonanimationfactory.h"

// framework
#include "engine/nodes/mesh.h"
#include "engine/nodes/scenegraph.h"
#include "engine/render/geometry.h"
#include "framework/gldevice.h"
#include "framework/framebuffer.h"
#include "tools/filestream.h"
#include "tools/profiling.h"
#include "image/image.h"

// ribbons
#include "ribbonanimation.h"

// cmath
#include "math.h"


//-----------------------------------------------------------------------------
/*!
   \param parent parent widget
*/
RibbonAnimationFactory::RibbonAnimationFactory()
{
}


//-----------------------------------------------------------------------------
/*!
*/
RibbonAnimationFactory::~RibbonAnimationFactory()
{
}


//-----------------------------------------------------------------------------
/*!
*/
void RibbonAnimationFactory::initGlStates()
{
   glDisable(GL_CULL_FACE);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(false);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


//-----------------------------------------------------------------------------
/*!
*/
void RibbonAnimationFactory::cleanUpGlStates()
{
   glEnable(GL_CULL_FACE);
}


//-----------------------------------------------------------------------------
/*!
*/
void RibbonAnimationFactory::initialize()
{
   RibbonAnimation::initialize();
}


//-----------------------------------------------------------------------------
/*!
   \param x field x position
   \param y field y position
*/
void RibbonAnimationFactory::add(float x, float y)
{
   x += 0.5f;
   y += 0.5f;

   RibbonAnimation* anim = new RibbonAnimation();
   Vector4 fieldPosition = Vector4(x, -y);
   anim->setFieldPosition(fieldPosition);
   mAnimations << anim;
}


//-----------------------------------------------------------------------------
/*!
*/
void RibbonAnimationFactory::unitTest1()
{
   mUnitTestTimer.setInterval(2000+(qrand() % 6000));
   add(qrand()%13, qrand()%11);
}


//-----------------------------------------------------------------------------
/*!
*/
void RibbonAnimationFactory::drawField()
{
   glBegin(GL_LINES);
   glColor4f(0.2f, 0.0f, 0.0f, 1.0f);

   glVertex3f(0.0f, 0.0f, 0.0f);
   glVertex3f(13.0f, 0.0f, 0.0f);

   glVertex3f(13.0f, 0.0f, 0.0f);
   glVertex3f(13.0f, 11.0f, 0.0f);

   glVertex3f(0.0f, 11.0f, 0.0f);
   glVertex3f(0.0f, 0.0f, 0.0f);

   glEnd();

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}


//-----------------------------------------------------------------------------
/*!
*/
void RibbonAnimationFactory::clearScreen()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


//-----------------------------------------------------------------------------
/*!
*/
void RibbonAnimationFactory::setupCamera()
{
   glLoadIdentity();

   // set camera
   Matrix lookAt= Matrix::lookAt(
      Vector(6.5f, -30.0f, 10.0f), // eye
      Vector(6.5f, 5.5f, 0.0f),    // center of field (13x11)
      Vector(0.0f, 0.0f, 1.0f)     // z is up
   );

   glMultMatrixf(lookAt);
}


//-----------------------------------------------------------------------------
/*!
  \param dt delta time
*/
void RibbonAnimationFactory::update(float dt)
{
   if (!mAnimations.isEmpty())
   {
      if (dt > 0.0)
      {
         initGlStates();

         QList<RibbonAnimation*> elapsedAnims;

         foreach (RibbonAnimation* anim, mAnimations)
         {
            anim->draw(dt*0.001);

            if (anim->isElapsed())
               elapsedAnims << anim;
         }

         foreach (RibbonAnimation* anim, elapsedAnims)
         {
            mAnimations.removeOne(anim);
            delete anim;
         }

         // drawField();

         cleanUpGlStates();
      }
   }
}


