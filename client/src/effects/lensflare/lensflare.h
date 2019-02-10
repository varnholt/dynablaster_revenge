#pragma once

// Qt
#include <QGLBuffer>

// math
#include "math/matrix.h"
#include "math/vector2.h"

// render
#include "render/texture.h"

// lensflare
#include "lensflareghost.h"
#include "lensflarevertex.h"


class LensFlare
{

   public:

      //! constructor
      LensFlare();

      //! destructor
      ~LensFlare();

      //! initializers for all
      static void initializeStatic();

      //! initializers per instance
      void initializeInstance();

      //! draw
      void draw();



      //! setter for sun position
      void setSun2d(const Vector2 &sun);

      //! getter for sun position
      Vector2 getSun2d() const;

      //! done prior to rendering
      static void pre();

      //! done after rendering
      static void post();

      //! bind texture
      void bindTexture();

      //! getter for initialized flag
      bool isInitialized() const;

      //! setter for initialized flag
      void setInitialized(bool initialized);

      //! getter for 3d sun
      const Vector &getSun3d() const;

      //! setter for 3d sun
      void setSun3d(const Vector & sun);

      //! getter for texture filename
      const QString& getTextureFileName() const;

      //! setter for texture filename
      void setTextureFileName(const QString &value);

      //! getter for ghost filename
      const QString& getGhostsFileName() const;

      //! setter for ghost filename
      void setGhostsFileName(const QString &value);

      //! getter for max length
      float getMaxLength() const;

      //! setter for max length
      void setMaxLength(float length);

      //! getter for lower limit
      float getLowerLimit() const;

      //! setter for lower limit
      void setLowerLimit(float value);

      //! getter for angle
      float getAngle() const;

      //! setter for angle
      void setAngle(float value);

      //! getter for invert flag
      bool isInverted() const;

      //! setter for inverted flag
      void setInverted(bool value);

      //! getter for invert offset
      float getInvertOffset() const;

      //! setter for invert offset
      void setInvertOffset(float value);

      //! check if sun coords are 2d
      bool is2d() const;

      //! setter for 2d sun coords
      void set2d(bool value);


   protected:

      //! compute texture offset
      Vector2 computeTextureOffset(
            int tileCol,
         int tileRow,
         float& tileWidth,
         float& tileHeight
      ) const;


      // handle buffers

      //! draw vertex buffer
      void drawVbos(
         unsigned int* vertexBuffer,
         unsigned int* indexBuffer,
         int indexCount
      );

      //! init vbos for sphere/ray
      void createBuffers();

      //! draw generated buffers
      void drawBuffers();

      //! rotate 2d vector
      void rotate(Vector2 &p, float angle);


      // handle ghosts

      //! deserialize ghosts
      void initializeGhosts();


      // members

      //! sun position
      Vector2 mSun2d;

      //! buffer used for vertices
      unsigned int mBufferVertices;

      //! buffer used for indices
      unsigned int mBufferIndices;

      //! true if initialized
      bool mInitialized;

      //! index count
      int mIndexCount;

      //! maximum length
      float mMaxLength;

      //! lower limit
      float mLowerLimit;

      //! global angle
      float mAngle;

      //! invert flag
      bool mInverted;

      //! invert offset
      float mInvertOffset;

      //! 2d
      bool m2d;

      //! ghosts
      QList<LensFlareGhost> mGhosts;

      //! time parameter
      int mTimeParam;

      //! sun parameter
      int mSunParam;

      //! length parameter
      int mNormalizedLength;

      //! texture
      Texture mTexture;

      //! sun vector
      Vector mSun3d;

      //! texture filename
      QString mTextureFileName;

      //! ghosts filename
      QString mGhostsFileName;

      // identical for all animations

      //! shader
      static unsigned int sShader;
};

