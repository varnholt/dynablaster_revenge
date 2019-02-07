/***************************************************************************
                              startalersfactory
 ---------------------------------------------------------------------------
    project              : startalers
    begin                : dec 2013
    copyright            : (C) 2013 by mueslee
    author               : mueslee
    email                : nope
***************************************************************************/


// header
#include "startalersfactory.h"

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
#include "startalers.h"

// cmath
#include "math.h"


//-----------------------------------------------------------------------------
/*!
   \param parent parent widget
*/
StarTalersFactory::StarTalersFactory(QObject *parent)
    : QObject(parent)
{
}


//-----------------------------------------------------------------------------
/*!
*/
StarTalersFactory::~StarTalersFactory()
{
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalersFactory::initialize()
{
   StarTalers::initializeStatic();
   initColors();
}


//-----------------------------------------------------------------------------
/*!
   \param x field x position
   \param y field y position
*/
void StarTalersFactory::add(float x, float y, Constants::ExtraType extra)
{
   x += 0.5f;
   y += 0.5f;

   StarTalers* anim = new StarTalers();
   Vector fieldPosition = Vector(x, -y);
   anim->setFieldPosition(fieldPosition);
   anim->setColor(getColor(extra));
   mAnimations << anim;
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalersFactory::initColors()
{
   /*
   bomb => white..green (0c7d03)
   flame => white..orange (ff8400)
   speedup => white..blue (0096ff)
   kick => white..pink?! (a200ff)
   skull => white..random?!
   */

   mColorBomb.set(0.047f, 0.49f, 0.012f);
   mColorFlame.set(1.0f, 0.518f, 0.0f);
   mColorSpeedUp.set(0.0f, 0.588f, 1.0f);
   mColorKick.set(0.635f, 0.0f, 1.0f);
   mColorDefault.set(0.0f, 0.0f, 0.0f);
}


//-----------------------------------------------------------------------------
/*!
   \param extra extra type
*/
const Vector &StarTalersFactory::getColor(Constants::ExtraType extra)
{
   switch (extra)
   {
      case Constants::ExtraBomb:
         return mColorBomb;
      case Constants::ExtraFlame:
         return mColorFlame;
      case Constants::ExtraSpeedup:
         return mColorSpeedUp;
      case Constants::ExtraKick:
         return mColorKick;
      default:
         return mColorDefault;
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalersFactory::unitTest1()
{
   mUnitTestTimer.setInterval(2000+(qrand() % 2000));
   add(qrand()%13, qrand()%11, Constants::ExtraSkull);
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalersFactory::drawField()
{
   glBegin(GL_LINES);
   glColor4f(0.2f, 0.0f, 0.0f, 1.0f);

   glVertex3f(0.0f, 0.0f, 0.1f);
   glVertex3f(13.0f, 0.0f, 0.1f);

   glVertex3f(13.0f, 0.0f, 0.1f);
   glVertex3f(13.0f, -11.0f, 0.1f);

   glVertex3f(0.0f, -11.0f, 0.1f);
   glVertex3f(0.0f, 0.0f, 0.1f);

   glEnd();

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalersFactory::setupCamera()
{
   glLoadIdentity();

   // set camera
   Matrix lookAt= Matrix::lookAt(
      Vector(6.5f, -30.0f, 20.0f), // eye
      Vector(6.5f, 5.5f, 0.0f),    // center of field (13x11)
      Vector(0.0f, 0.0f, 1.0f)     // z is up
   );

   glMultMatrixf(lookAt);
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalersFactory::initGlStates()
{
   glDisable(GL_CULL_FACE);

//   glDisable(GL_DEPTH_TEST);
   glDepthMask(false);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalersFactory::cleanUpGlStates()
{
   glEnable(GL_CULL_FACE);

//   glEnable(GL_DEPTH_TEST);
   glDepthMask(true);

   glDisable(GL_BLEND);
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalersFactory::update(float dt)
{
   if (!mAnimations.isEmpty())
   {
      if (dt > 0.0)
      {
         initGlStates();

         // setupCamera();

         QList<StarTalers*> elapsedAnims;

         // render startalers

         // dump projection matrix
         Matrix camera;
         glGetFloatv(GL_PROJECTION_MATRIX, camera.data());

         StarTalers::setCamera(camera);
         StarTalers::pre();

         foreach (StarTalers* anim, mAnimations)
         {
            if (!anim->isInitialized())
            {
               // qDebug("anim %p initialized", anim);
               anim->initializeInstance();
            }

            anim->draw(dt * 0.05);

            if (anim->isElapsed())
               elapsedAnims << anim;
         }

         StarTalers::post();

         // clean up elapsed animations
         foreach (StarTalers* anim, elapsedAnims)
         {
            // qDebug("anim %p deleted after %f", anim, anim->getTime());
            mAnimations.removeOne(anim);
            delete anim;
         }

         // drawField();

         cleanUpGlStates();
      }
   }
}

