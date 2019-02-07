#ifndef GLWIDGET_H
#define GLWIDGET_H

// base
#include "drawable.h"

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

class SphereFragmentsDrawable : public QObject, public Drawable
{
    Q_OBJECT
    
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
};

#endif // GLWIDGET_H
