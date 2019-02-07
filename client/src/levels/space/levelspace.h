#pragma once

#include "../level.h"
#include "materials/materialfactory.h"

class BlurFilter;
class FrameBuffer;
class RenderDevice;
class EarthBackground;
class DuplicateAlpha;
class BlendQuad;
class StarFieldBackground;

class LevelSpace : public Level, public MaterialFactory
{

public:

   LevelSpace();
   virtual ~LevelSpace();

   virtual void initialize();
   virtual void drawBackground();
   virtual void animate(float dt);
   virtual void reset();
   virtual void loadData();

   Material* createMaterial(SceneGraph* scene, int materialId) const;

private:

   //! setup project matrix
   void projectionSetup();

   void updateFps();

   EarthBackground* mEarth;   //!< sphere fragment container
   float mEarthRotation;
   Vector mCamera;            //!< camera vector
   StarFieldBackground* mStarField;
};
