#pragma once

#include "tools/array.h"
#include "math/matrix.h"

class Node;

class BakedTransformation : public Array<Matrix>
{
public:
   BakedTransformation();
   BakedTransformation(Node *node, float stepSize);

   Matrix interpolate(float frame) const;

private:
   float mStepSize;
};
