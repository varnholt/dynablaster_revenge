// header
#include "gamelogodrawable.h"
#include "gamelogopointsprite.h"
#include "menus/defaultshader.h"
#include "menus/psdlayer.h"

// engine
#include "gldevice.h"
#include "math/matrix.h"
#include "tools/random.h"

// cmath
#include <math.h>
#include <string.h>

// the real page-name string MenuDrawable's pageChanged(QString) signal carries for the actual
// main menu page (client/src/menus/gamemenudefines.h's MAINMENU - that header itself isn't
// ported, it's all networking/gameplay action-name constants unrelated to rendering).
#define MAINMENU "data/menus/mainmenu.psd"

#define SPARK_COUNT 300
#define SPARK_START_INTENSITY 0.06f

#define LAYER_DYNABLASTER "dynablaster"
#define LAYER_REVENGE "revenge"

#define FADE_IN_LENGTH 80.0f
#define FADE_OUT_LENGTH 80.0f

GameLogoDrawable::GameLogoDrawable(RenderDevice* dev, bool visible)
    : SphereFragmentsDrawable(dev, visible),
      mMainMenuVisible(true),
      mDeltaTime(0.0f),
      mTime(0.0f),
      mLayerDynablaster(0),
      mLayerRevenge(0),
      mFadeInEnd(0.0f),
      mFadeOutEnd(0.0f),
      mSparkTimesInitialized(false)
{
   mFilename = "data/logo/logo.psd";
}

GameLogoDrawable::~GameLogoDrawable()
{
   for (int i = 0; i < mLayers.size(); i++)
      delete mLayers[i];
}

void GameLogoDrawable::initializeGL()
{
   initializeLayers();
   initializeSparks();

   SphereFragmentsDrawable::initializeGL();
}

void GameLogoDrawable::paintGL()
{
   updateFadeAlpha();

   if (mAlpha > 0.0f)
   {
      SphereFragmentsDrawable::paintGL();

      initOrthoGlParameters();

      mLayerDynablaster->render(20.0f * cos(mTime * 0.03f), 30.0f + 15.0f * sin(mTime * 0.04f), mAlpha);

      mLayerRevenge->render(30.0f * cos(mTime * 0.03f), 30.0f + 25.0f * sin(mTime * 0.04f), mAlpha);

      initPointSpriteGlParameters();
      drawSparks();

      cleanupGlParameters();
   }
}

void GameLogoDrawable::pageChanged(const std::string& page)
{
   bool wasVisible = mMainMenuVisible;
   mMainMenuVisible = (page == MAINMENU);

   // fade out
   if (wasVisible && !mMainMenuVisible)
   {
      mFadeOutEnd = mTime + FADE_OUT_LENGTH;
   }

   // fade in
   else if (!wasVisible && mMainMenuVisible)
   {
      mFadeInEnd = mTime + FADE_IN_LENGTH;
   }
}

void GameLogoDrawable::animate(float time)
{
   mDeltaTime = time - mTime;
   mTime = time;
}

void GameLogoDrawable::setVisible(bool visible)
{
   Drawable::setVisible(visible);

   if (!visible)
      mSparkTimesInitialized = false;
}

void GameLogoDrawable::drawSparks()
{
   bool visible = true;

   Array<Vector> positions;
   Array<float> glow;

   if (!mSparkTimesInitialized)
   {
      for (int i = 0; i < mSparks.size(); i++)
         mSparks[i].mStartTime = mTime + frand(100.0f);

      mSparkTimesInitialized = true;
   }

   for (int i = 0; i < mSparks.size(); i++)
   {
      if (mSparks[i].mStartTime <= mTime)
      {
         // reset spark if intensity threshold exceeded
         if (mSparks[i].mIntensity < 0.01f)
         {
            initSpark(mSparks[i]);
            mSparks[i].mStartTime = mTime + frand(100.0f);
         }

         mSparks[i].mDirection.y -= mDeltaTime * 0.02f;
         mSparks[i].mScalar += mDeltaTime * 0.02f;
         mSparks[i].mIntensity -= mDeltaTime * 0.0015f;

         Vector pos = mSparks[i].mOrigin + (mSparks[i].mDirection * mSparks[i].mLength * mSparks[i].mScalar);

         // TODO: fix this (kept verbatim from the original - see gamelogodrawable.cpp)
         pos.z = -12.0f;

         // spark fading
         float intensity = 1.0f;
         if (mFadeInEnd > mTime)
         {
            intensity = 1.0f - (mFadeInEnd - mTime) / FADE_IN_LENGTH;
         }
         else if (mFadeOutEnd > mTime)
         {
            intensity = (mFadeOutEnd - mTime) / FADE_OUT_LENGTH;
         }
         else if (!mMainMenuVisible)
         {
            visible = false;
         }

         if (visible)
         {
            positions.add(pos);
            glow.add(mSparks[i].mIntensity * intensity);
         }
      }
   }

   if (visible)
   {
      GameLogoPointSprite::setPointSprites(positions, glow);

      GameLogoPointSprite::draw();
   }
}

void GameLogoDrawable::updateFadeAlpha()
{
   float alpha = mMainMenuVisible ? 1.0f : 0.0f;

   if (mFadeInEnd > mTime)
   {
      alpha = 1.0f - (mFadeInEnd - mTime) / FADE_IN_LENGTH;
   }
   else if (mFadeOutEnd > mTime)
   {
      alpha = (mFadeOutEnd - mTime) / FADE_OUT_LENGTH;
   }

   mAlpha = alpha;
}

void GameLogoDrawable::initOrthoGlParameters()
{
   static_cast<GLDevice*>(mDevice)->setProjectionMatrix(
      Matrix::ortho(0.0f, (float)mPsd.getWidth(), (float)mPsd.getHeight(), 0.0f, 300.0f, -300.0f)
   );

   // PSDLayer::render() (see menus/psdlayer.cpp) draws through whichever shader the caller has
   // already bound - matches the same convention MenuDrawable/MenuPage established for every
   // other PSD-backed item draw.
   mDevice->setShader(getDefaultMenuShader());

   // enable blending
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);
}

void GameLogoDrawable::initPointSpriteGlParameters()
{
   float znear = 0.1f;
   float zfar = 5000.0f;

   float scale = 0.5f;
   float aspect = 16.0f / 9.0f;

   float ymin = znear * scale;
   float ymax = -ymin;

   float xmax = ymax * aspect;
   float xmin = ymin * aspect;

   static_cast<GLDevice*>(mDevice)->setProjectionMatrix(Matrix::frustum(xmin, xmax, ymin, ymax, znear, zfar));

   // init blending (additive glow)
   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);
   glEnable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ONE);

   // GameLogoPointSprite::draw() binds its own shader.
   mDevice->setShader(0);
}

void GameLogoDrawable::cleanupGlParameters()
{
   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GameLogoDrawable::initializeLayers()
{
   mPsd.load(mFilename.toStdString().c_str());

   // assign layers to the two named pointers this class actually renders (see the header
   // comment - "earth"/"highlight" layers are still constructed here, matching the original 1:1,
   // even though nothing renders them: harmless data loading, not GL-drawing code, so left as-is
   // per this port's own scope rule).
   for (int l = 0; l < mPsd.getLayerCount(); l++)
   {
      PSD::Layer* psdlayer = mPsd.getLayer(l);

      PSDLayer* layer = new PSDLayer(psdlayer);

      if (strcmp(psdlayer->getName(), LAYER_DYNABLASTER) == 0)
      {
         mLayerDynablaster = layer;
      }
      else if (strcmp(psdlayer->getName(), LAYER_REVENGE) == 0)
      {
         mLayerRevenge = layer;
      }

      mLayers.add(layer);
   }
}

void GameLogoDrawable::initSpark(Spark& spark)
{
   spark.mOrigin = mSparkOrigin;  // maybe vary a little
   spark.mPosition = mSparkOrigin;
   spark.mIntensity = SPARK_START_INTENSITY;
   spark.mScalar = 0.0f;

   float originRandX = frand(0.2f);
   float originRandY = frand(0.2f);
   float originRandZ = -0.2f + frand(0.2f);

   spark.mOrigin.x += originRandX;
   spark.mOrigin.y += originRandY;
   spark.mOrigin.z += originRandZ;

   float dirRandX = -0.75f + frand(0.5f);
   float dirRandY = -0.75f + frand(0.5f);

   spark.mDirection = Vector(-1.0f - dirRandX, -1.0f - dirRandY, -12.0f);

   spark.mLength = 1.0f;
}

void GameLogoDrawable::initializeSparks()
{
   GameLogoPointSprite::initialize();

   mSparkOrigin = Vector(-3.7f, -3.3f, -12.0f);

   for (int i = 0; i < SPARK_COUNT; i++)
   {
      Spark spark;
      initSpark(spark);
      mSparks.add(spark);
   }
}
