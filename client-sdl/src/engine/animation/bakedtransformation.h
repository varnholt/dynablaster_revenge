#pragma once

#include "math/matrix.h"
#include "tools/array.h"

class Node;

class BakedTransformation : public Array<Matrix>
{
public:
   BakedTransformation();
   BakedTransformation(Node* node, float stepSize);

   Matrix interpolate(float frame) const;

private:
   float mStepSize;
};
