#pragma once

// base
#include "framework/drawable.h"

//
#include "framework/globaltime.h"
#include "spherefragment.h"

class BombFuzeGeometryVbo;
class BombSocketGeometryVbo;
class BlurFilter;
class Geometry;
class Mesh;
class RenderDevice;
class SceneGraph;
class SphereFragmentContainer;
class SphereGeometryVbo;
class DuplicateAlpha;
class BlendQuad;
class FrameBuffer;

class SphereFragmentsDrawable : public Drawable
{
public:
   SphereFragmentsDrawable(RenderDevice* dev, bool visible = false);

   virtual ~SphereFragmentsDrawable();

   virtual void initializeGL();

   virtual void paintGL();

   void removeFragments();

protected:
   //! setup project matrix
   void projectionSetup();

   //! draw bomb parts
   void drawBombParts();

   //! scene graph for earth
   SceneGraph* mSceneGraphEarth;

   //! scene graph for bomb parts
   SceneGraph* mSceneGraphBomb;

   //! bomb vbo
   SphereGeometryVbo* mBomb;

   //! fuze vbo
   BombFuzeGeometryVbo* mFuze;

   //! socket vbo
   BombSocketGeometryVbo* mSocket;

   //! sphere fragment container
   SphereFragmentContainer* mFragmentContainer;

   //! blur filter
   BlurFilter* mBlur;

   //! alpha to color duplication
   DuplicateAlpha* mAlphaDuplicate;

   //! blend quads
   BlendQuad* mBlendQuad;

   //! camera vector
   Vector mCamera;

   //! fading
   float mAlpha;

   //! position offset
   Vector mPositionOffset;

   //! scale
   float mScale;

   // the legacy version pulled these three render targets from MainDrawable::getInstance()->
   // getRenderBuffer(N) - a shared pool owned by the QGLWidget hosting the game. This port has no
   // MainDrawable, so SphereFragmentsDrawable owns its own trio instead, sized to match the main
   // viewport lazily on first use (see paintGL()).
   FrameBuffer* mEarthFb;
   FrameBuffer* mAuraFb;
   FrameBuffer* mBombFb;
};
