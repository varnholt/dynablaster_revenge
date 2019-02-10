#ifndef RIBBONANIMATION_H
#define RIBBONANIMATION_H

// Qt
#include <QGLBuffer>

// framework
#include "math/vector4.h"
#include "render/texture.h"
#include "ribbonvertex.h"


class RibbonAnimation
{

   public:

      //! constructor
      RibbonAnimation();

      //! this is only done once
      static void initializeTexture();

      //! initialize texture
      static void initialize();

      //! draw
      void draw(float dt);

      //! setter for field position
      void setFieldPosition(const Vector4& fieldPosition);

      //! getter for field position
      Vector4 getFieldPosition() const;

      //! getter for elapsed time
      bool isElapsed() const;


   protected:

      //! update time
      void updateTime(float dt);

      //! init vertex buffer
      static void initVbos();

      //! draw vertex buffer
      void drawVbos();

      //! animation time
      float mTime;

      //! field position
      Vector4 mFieldPosition;

      // vertex and index buffer objects
      static unsigned int mVertexBuffer;
      static unsigned int mIndexBuffer;

      static QVector<RibbonVertex> mVertices;
      static QVector<GLuint> mIndices;

      //! shader
      static unsigned int mShader;

      //! time parameter
      static int mTimeParam;

      //! field position
      static int mFieldPositionParam;

      //! circle offset
      static int mCircleOffsetParam;

      //! texture
      static Texture sTexture;
};

#endif


