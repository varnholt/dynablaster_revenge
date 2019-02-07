#ifndef GAMELOGODRAWABLE_H
#define GAMELOGODRAWABLE_H

#include "drawable.h"

// engine
#include "image/psd.h"
#include "math/vector.h"
#include "render/texture.h"

// Qt
#include <QtOpenGL/QGLWidget>
#include <QImage>
#include <QObject>
#include <QList>

// effects
#include "effects/spherefragments/spherefragmentsdrawable.h"

// defines
#define CUBE_COUNT 6

// forward declarations
class QGLBuffer;
class SceneGraph;
class PSDLayer;

class GameLogoDrawable : public SphereFragmentsDrawable
{
   Q_OBJECT

   public:

      class Spark
      {
         public:

            Spark()
               : mPointSize(1.0f),
                 mIntensity(1.0f),
                 mLength(0.0f),
                 mScalar(0.0f),
                 mStartTime(0.0f)
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

      class Cube
      {
         public:

            Cube()
               : mScale(1.0f),
                 mRotOffset(0.0f),
                 mRotSpeed(0.0f),
                 mRotX(0.0f),
                 mRotY(0.0f),
                 mRotZ(0.0f)
            {
            }

            float mScale;
            float mRotOffset;
            float mRotSpeed;
            float mRotX;
            float mRotY;
            float mRotZ;
      };

      class Vertex3D
      {
         public:

            Vertex3D()
              : mU(0.0f),
                mV(0.0f)
            {
            }

            Vector mPosition;
            Vector mNormal;
            float mU;
            float mV;
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

      //! load cube object
      void initializeCubes();

      //! initialize sparks
      void initializeSparks();

      //! init gl parameters
      void initOrthoGlParameters();

      //! init frustum parameters
      void initFrustumGlParameters(bool front);

      //! init pointsprite gl parameters
      void initPointSpriteGlParameters();

      //! cleanup gl parameters
      void cleanupGlParameters();


      // main drawing

      //! set alpha for psd layers
      void updateFadeAlpha();

      //! draw point sprites
      void drawSparks();

      //! draw cubes
      void drawCubes();

      //! draw highlight
      void drawHighlight();

      //! paint vbos
      void drawVbos(
         QGLBuffer *vertexBuffer,
         QGLBuffer *indexBuffer,
         int indicesCount
      );


      //
      void initSpark(Spark &spark);


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

      //! earth texture
      PSDLayer* mLayerEarth;

      //! font texture
      PSDLayer* mLayerDynablaster;

      //! font texture
      PSDLayer* mLayerRevenge;

      //! all layers
      QList<PSDLayer*> mLayers;

      // mesh members

      //! scene graph
      SceneGraph* mSceneGraphEarth;

      //
      QGLBuffer* mVertexBufferCube;
      QGLBuffer* mIndexBufferCube;
      QVector<Vertex3D> mVerticesCube;
      QVector<GLuint> mIndicesCube;

      //
      float mFadeInEnd;
      float mFadeOutEnd;

      // spark point sprites

      QList<Spark> mSparks;
      Vector mSparkOrigin;
      bool mSparkTimesInitialized;

      // list of cubes
      QList<Cube> mCubes;

      // textures

      //! cube texture
      Texture mCubeTexture[CUBE_COUNT];

      // highlighting

      //! highlight texture
      PSDLayer* mTextureHighlight;

      //! highlight duration
      float mHighlightDuration;
};

#endif // GAMELOGODRAWABLE_H
