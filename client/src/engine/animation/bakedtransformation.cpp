#include "bakedtransformation.h"
#include "nodes/node.h"
#include <math.h>

BakedTransformation::BakedTransformation()
: Array<Matrix>()
, mStepSize( 0.0f )
{
}

BakedTransformation::BakedTransformation(Node *node, float stepSize)
: Array<Matrix>()
, mStepSize( 1.0f / stepSize )
{
   int maxFrame= node->getAnimationLength();
   int keys= (int) ceil(maxFrame/stepSize);
   if (keys == 0) keys=1;

   this->init(keys);

   for (int i=0; i<keys; i++)
   {
      node->transform( i*stepSize );
      this->add( node->getTransform() );
   }
}

Matrix BakedTransformation::interpolate(float frame) const
{
   if (frame <= 0.0f)
      return this->get(0);

   frame *= mStepSize;
   int index= (int)floor(frame);

   // make sure [index] and [index+1] exist for blending
   if (index < this->size()-1)
   {
      float t= 1.0f - (index + 1 - frame);
      const Matrix& m1= this->get(index);
      const Matrix& m2= this->get(index+1);
      return Matrix::blend(m1, m2, t);
   }
   else
   {
      // last index
      if (this->size()>0)
         return this->getLast();
   }

   // none of the above paths: track is empty, so return identity matrix
   return Matrix();
}

