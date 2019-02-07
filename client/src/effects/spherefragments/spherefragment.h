#pragma once

// engine
#include "math/vector.h"
#include "math/matrix.h"
#include "tools/array.h"
#include "framework/gldevice.h"

class Mesh;
class Image;

class SphereFragment
{
public:

   //! constructor
   SphereFragment(const Array<Mesh*>& meshList, Image* orderImage);

   int getPartCount() const;

   void animate(float time, const Matrix& rotation);

   Matrix* getMatrices() const;

   float* getFresnelFactors() const;

   //! draw single fragment
   void draw();


protected:
   unsigned int mVertexBuffer;
   unsigned int mIndexBuffer;
   int          mVertexCount;
   int          mIndexCount;
   Array<Matrix> mMatrix;
   Array<float>  mRandom;
   Array<float>  mTime;
   Array<Matrix> mModelView;
   Array<float>  mFresnelFactors;
};
