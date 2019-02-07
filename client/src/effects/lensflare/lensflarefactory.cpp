// header
#include "lensflarefactory.h"

// framework
#include "engine/nodes/mesh.h"
#include "engine/nodes/scenegraph.h"
#include "engine/render/geometry.h"
#include "framework/gldevice.h"
#include "framework/framebuffer.h"
#include "tools/filestream.h"
#include "tools/profiling.h"
#include "image/image.h"

// game
#include "game/videodebugging.h"

// ribbons
#include "lensflare.h"

// cmath
#include "math.h"

// Qt
#include <QSettings>

// static
LensFlareFactory* LensFlareFactory::sLensFlareFactory = 0;

//-----------------------------------------------------------------------------
/*!
   \param parent parent widget
*/
LensFlareFactory::LensFlareFactory()
 : QObject()
{
   sLensFlareFactory = this;

   QSettings settings(
      "data/effects/lensflare/flares/flares.ini",
      QSettings::IniFormat
   );

   QStringList groups = settings.value("main/groups").toStringList();

   foreach (const QString& group, groups)
   {
      QString texture = settings.value(QString("%1/texture").arg(group)).toString();
      QString ghosts = settings.value(QString("%1/ghosts").arg(group)).toString();

      Vector sun(
         settings.value(QString("%1/sun_x").arg(group)).toFloat(),
         settings.value(QString("%1/sun_y").arg(group)).toFloat(),
         settings.value(QString("%1/sun_z").arg(group)).toFloat()
      );

      float maxLength = settings.value(QString("%1/maxlength").arg(group)).toFloat();
      float lowerLimit = settings.value(QString("%1/lowerlimit").arg(group)).toFloat();
      float angle = settings.value(QString("%1/angle").arg(group)).toFloat();

      bool inverted = false;
      float invertOffset = settings.value(QString("%1/invert").arg(group)).toFloat(&inverted);
      bool sunIs2d = settings.value(QString("%1/2d").arg(group), false).toBool();

      LensFlare* lensFlare = new LensFlare();
      lensFlare->setTextureFileName(texture);
      lensFlare->setGhostsFileName(ghosts);
      lensFlare->setMaxLength(maxLength);
      lensFlare->setLowerLimit(lowerLimit);
      lensFlare->setSun3d(sun);
      lensFlare->setAngle(angle);
      lensFlare->setInverted(inverted);
      lensFlare->setInvertOffset(invertOffset);
      lensFlare->set2d(sunIs2d);

      mLensFlares.insert(group, lensFlare);
   }

   // activate default flare
   // activate("castle");
}


//-----------------------------------------------------------------------------
/*!
*/
LensFlareFactory::~LensFlareFactory()
{
   qDeleteAll(mLensFlares.values());
   mLensFlares.clear();
}


//-----------------------------------------------------------------------------
/*!
*/
LensFlareFactory *LensFlareFactory::getInstance()
{
   if (!sLensFlareFactory)
      new LensFlareFactory();

   return sLensFlareFactory;
}


//-----------------------------------------------------------------------------
/*!
*/
void LensFlareFactory::initialize()
{
   LensFlare::initializeStatic();

   foreach (LensFlare* flare, mLensFlares.values()) {
      flare->initializeInstance();
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void LensFlareFactory::drawField()
{
   glBegin(GL_LINES);
   glColor4f(0.2f, 0.0f, 0.0f, 1.0f);

   glVertex3f(0.0f, 0.0f, 0.0f);
   glVertex3f(13.0f, 0.0f, 0.0f);

   glVertex3f(13.0f, 0.0f, 0.0f);
   glVertex3f(13.0f, 11.0f, 0.0f);

//   glVertex3f(13.0f, 11.0f, 0.0f);
//   glVertex3f(0.0f, 11.0f, 0.0f);

   glVertex3f(0.0f, 11.0f, 0.0f);
   glVertex3f(0.0f, 0.0f, 0.0f);

   glEnd();

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}


//-----------------------------------------------------------------------------
/*!
*/
void LensFlareFactory::drawCenter()
{
   glBegin(GL_LINES);

   glVertex2f(-0.01f, 0.0f);
   glVertex2f(0.01f, 0.0f);

   glVertex2f(0.0f, -0.01f);
   glVertex2f(0.0f, 0.01f);

   glEnd();
}


//-----------------------------------------------------------------------------
/*!
   \param x x pos
   \param y y pos
*/
void LensFlareFactory::drawFlareLine(float x, float y)
{
   glBegin(GL_LINES);

   glVertex2f(x, y);
   glVertex2f(0.0f, 0.0f);

   glEnd();
}


//-----------------------------------------------------------------------------
/*!
*/
void LensFlareFactory::drawSun()
{
   glBegin(GL_LINES);

   glColor4f(0.7f, 0.7f, 0.0f, 1.0f);

   glVertex3f(mActive->getSun3d().x,        mActive->getSun3d().y,        mActive->getSun3d().z - 0.5f);
   glVertex3f(mActive->getSun3d().x,        mActive->getSun3d().y,        mActive->getSun3d().z + 0.5f);

   glVertex3f(mActive->getSun3d().x - 0.5f, mActive->getSun3d().y,        mActive->getSun3d().z       );
   glVertex3f(mActive->getSun3d().x + 0.5f, mActive->getSun3d().y,        mActive->getSun3d().z       );

   glVertex3f(mActive->getSun3d().x,        mActive->getSun3d().y - 0.5f, mActive->getSun3d().z       );
   glVertex3f(mActive->getSun3d().x,        mActive->getSun3d().y + 0.5f, mActive->getSun3d().z       );

   glEnd();

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}


//-----------------------------------------------------------------------------
/*!
   \param aspectY y aspect
   \param x x coord to fill
   \param y y coord to fill
   \param
*/
void LensFlareFactory::make2d(
   float aspectY,
   float& x,
   float& y,
   float& xTranslate,
   float& yTranslate
)
{
   Matrix projMatrix;

   glGetFloatv(GL_PROJECTION_MATRIX, projMatrix.data());

   Vector4 lightPos = Vector4(
      mActive->getSun3d().x,
      mActive->getSun3d().y,
      mActive->getSun3d().z
   );

   Vector4 projLight = projMatrix * lightPos;

   float lightX = projLight.x / projLight.w;
   float lightY = (projLight.y / projLight.w) * aspectY;

   // qDebug("%f, %f", lightX, lightY);

   x = lightX;
   y = lightY;

   xTranslate = projMatrix.xw;
   yTranslate = projMatrix.yw;
}


//-----------------------------------------------------------------------------
/*!
*/
void LensFlareFactory::drawSun2dCoords(float x, float y)
{
   glBegin(GL_LINES);
   glVertex2f(x + 0.01, y);
   glVertex2f(x - 0.01, y);
   glVertex2f(x, y - 0.01);
   glVertex2f(x, y + 0.01);
   glEnd();
}


//-----------------------------------------------------------------------------
/*!
*/
void LensFlareFactory::draw()
{
   // VideoDebugging::debugModelViewMatrix();
   // VideoDebugging::debugProjectionMatrix();

   // animateSun();
   // drawSun();
   // drawField();

   float aspectY = 9.0f / 16.0f;
   float xTranslate = 0.0f;
   float yTranslate = 0.0f;

   float x,y;
   make2d(aspectY, x, y, xTranslate, yTranslate);

   glMatrixMode(GL_PROJECTION);

   glPushMatrix();
   glLoadIdentity();

   glOrtho(
      -1.0f,
       1.0f,
      -aspectY,
       aspectY,
     -10.0f,
      10.0f
   );

   // drawSun2dCoords(x, y);
   // drawCenter();
   // drawFlareLine(x, y);

   LensFlare::pre();

   if (!mActive->isInitialized())
      mActive->initializeInstance();

   if (mActive->is2d())
   {
      const Vector& sun = mActive->getSun3d();
      mActive->setSun2d(Vector2(sun.x + (xTranslate * 0.1f) + 0.5f, sun.y * aspectY));
   }
   else
   {
      mActive->setSun2d(Vector2(x, y));
   }

   mActive->draw();

   LensFlare::post();

   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
}


//-----------------------------------------------------------------------------
/*!
   \param level flare group
   \return \c true if activated
*/
bool LensFlareFactory::activate(const QString &key)
{
   bool activated = false;

   QMap<QString, LensFlare*>::iterator it = mLensFlares.find(key);

   if (it != mLensFlares.end())
   {
      mActive = it.value();
      activated = true;
   }

   return activated;
}


//-----------------------------------------------------------------------------
/*!
   \return all flare keys
*/
QList<QString> LensFlareFactory::getKeys() const
{
   return mLensFlares.keys();
}

