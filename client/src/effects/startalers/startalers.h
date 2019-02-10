#pragma once

// Qt
#include <QGLBuffer>
#include <QTime>

// framework
#include "math/matrix.h"
#include "math/vector2.h"
#include "render/texture.h"

// rays
#include "startalersvertex.h"

class StarTalers
{

   public:

      //! constructor
      StarTalers();

      //! destructor
      ~StarTalers();

      //! initializers for all
      static void initializeStatic();

      //! initializers per instance
      void initializeInstance();

      //! draw
      void draw(float dt);

      //! setter for field position
      void setFieldPosition(const Vector& fieldPosition);

      //! getter for field position
      Vector getFieldPosition() const;

      //! setter for color
      void setColor(const Vector& color);

      //! getter for color
      const Vector& getColor() const;

      //! getter for elapsed time
      bool isElapsed() const;

      //! prepare rendering
      static void pre();

      //! cleanup after rendering
      static void post();

      //! bind texture
      static void bindTexture();

      //! getter for initialized flag
      bool isInitialized() const;

      //! setter for initialized flag
      void setInitialized(bool initialized);

      //! getter for time
      float getTime() const;

      //! setter for camera
      static void setCamera(const Matrix& cam);

      //! getter for camera
      static const Matrix& getCamera();

      //! prepare camera
      static void prepareCamera();


protected:

      //! make a ray
      Vector computeDirection(float width, float depth, float height);

      //! compute speed
      float computeSpeed(float min, float range);

      //! compute texture offset
      Vector2 computeTextureOffset() const;

      //! update time
      void updateTime(float dt);

      //! draw vertex buffer
      void drawVbos(
         unsigned int* vertexBuffer,
         unsigned int* indexBuffer,
         int indexCount
      );

      //! init vbos for sphere/ray
      void createBuffers(float scale = 1.0f);

      //! draw generated buffers
      void drawBuffers();

      //! generate height map
      void generateHeightmap();

      //! generate test heightmap
      void generateTestHeightmap();

      //! debug height map
      void debugHeightmap();


      //! animation time
      float mTime;

      //! field position
      Vector mFieldPosition;

      //! heightmap for this a
      float* mHeightmap;

      //! vertices buffer
      unsigned int mBufferVertices;

      //! indices buffer
      unsigned int mBufferIndices;

      //! initialized flag
      bool mInitialized;

      //! index count
      int mIndexCount;

      //! color
      Vector mColor;


      // identical for all animations

      //! camera
      static Matrix sCamera;

      //! shader
      static unsigned int mShader;

      //! time parameter shader param
      static int mTimeParam;

      //! texture
      static Texture sTexture;

      //! rotation shader param
      static int sRotationParam;

      //! field position shader param
      static int sFieldParam;

      //! heightmap parameter
      static int sHeightmapParam;

      //! camera parameter
      static int sCameraParam;

      //! color param
      static int sColorParam;
};

