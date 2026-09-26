#pragma once

// GLES3 port of client/src/game/countdowndrawable.h. Interface unchanged from the original
// except dropping mLayerIds (declared, never read or written anywhere in the original) and the
// unused <QImage> include.

#include "drawable.h"

#include <memory>
#include <string>
#include <vector>

#include "image/psd.h"

class PSDLayer;

class CountdownDrawable : public Drawable
{
public:
   CountdownDrawable(RenderDevice* dev);
   virtual ~CountdownDrawable();

   void initializeGL();
   void paintGL();
   void animate(float time);

   void countdown(int left);

protected:
   void drawCountdown();
   void initializeLayers();
   void initGlParameters();
   void cleanupGlParameters();

   PSD mPsd;
   std::vector<std::unique_ptr<PSDLayer>> mPsdLayers;
   std::vector<float> mLayerAlphas;
   std::string mFilename;

   int mTimeLeft;
   float mAnimationStartTime;
   bool mAnimationActive;
   float mDeltaTime;
   float mTime;
   bool mDeltaTimeInitialized;

   // GLES3 port addition - the shared per-item menu shader (see menus/defaultshader.h), bound
   // once per paintGL() call instead of the original's `mDevice->setShader(0)` (desktop GL's
   // fixed-function fallback, which GLES3 has no equivalent of).
   unsigned int mShader;
};
