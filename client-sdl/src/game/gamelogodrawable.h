#pragma once

// GLES3 port of client/src/game/gamelogodrawable.h.
//
// Real scope note: the shipped game's GameLogoDrawable::paintGL() has its rotating-cube pass
// (initFrustumGlParameters()/drawCubes(), both calls) and its ortho earth-layer/highlight pass
// (initOrthoGlParameters()/mLayerEarth->render()/drawHighlight()) entirely commented out in the
// real source - confirmed by reading the original .cpp directly, not assumed. Only three things
// actually run every frame: SphereFragmentsDrawable::paintGL() (the earth/bomb/fragments effect,
// already ported), the "dynablaster"/"revenge" PSD text overlays, and the spark point sprites.
// Per [[feedback_destructive_fork_approach]] ("don't preserve dead abstractions... gut them with a
// comment explaining why, don't port them"), the cube system (Cube/Vertex3D inner classes,
// initializeCubes()/drawCubes(), the cube.hjb mesh, the 6 cube_N textures) and the dead earth/
// highlight ortho pass (mLayerEarth/mTextureHighlight/drawHighlight()) are not ported here.

#include "effects/spherefragments/spherefragmentsdrawable.h"

// engine
#include "image/psd.h"
#include "math/vector.h"
#include "tools/array.h"

#include <QObject>
#include <QString>

class PSDLayer;

class GameLogoDrawable : public SphereFragmentsDrawable
{
   Q_OBJECT

public:
   class Spark
   {
   public:
      Spark() : mPointSize(1.0f), mIntensity(1.0f), mLength(0.0f), mScalar(0.0f), mStartTime(0.0f)
      {
      }

      Vector mOrigin;
      Vector mDirection;
      Vector mPosition;

      float mPointSize;
      float mIntensity;
      float mLength;
      float mScalar;
      float mStartTime;
   };

   //! constructor
   GameLogoDrawable(RenderDevice* dev, bool visible = false);

   //! destructor
   virtual ~GameLogoDrawable();

   //! initialize
   void initializeGL();

   //! draw
   void paintGL();

   //! animate
   void animate(float time);

   //! overwrite base
   void setVisible(bool visible);

public slots:

   //!
   void pageChanged(const QString& page);

protected:
   //! initialize layers
   void initializeLayers();

   //! initialize sparks
   void initializeSparks();

   //! init ortho gl parameters
   void initOrthoGlParameters();

   //! init pointsprite gl parameters
   void initPointSpriteGlParameters();

   //! cleanup gl parameters
   void cleanupGlParameters();

   // main drawing

   //! set alpha for psd layers
   void updateFadeAlpha();

   //! draw point sprites
   void drawSparks();

   //
   void initSpark(Spark& spark);

   //
   bool mMainMenuVisible;

   // animation

   //! time
   float mDeltaTime;
   float mTime;

   // overlay members

   //! psd instance
   PSD mPsd;

   //! filename to load from
   QString mFilename;

   //! font texture
   PSDLayer* mLayerDynablaster;

   //! font texture
   PSDLayer* mLayerRevenge;

   //! all layers (owns them - see destructor)
   Array<PSDLayer*> mLayers;

   //
   float mFadeInEnd;
   float mFadeOutEnd;

   // spark point sprites

   Array<Spark> mSparks;
   Vector mSparkOrigin;
   bool mSparkTimesInitialized;
};
