#pragma once

#include "math/matrix.h"
#include "tools/array.h"
#include "tools/list.h"
#include "tools/streamable.h"
#include "weight.h"

class Bone : public Streamable
{
public:
   Bone();
   Bone(const Bone& bone);
   Bone(const Bone& bone, Array<int>* remap);
   virtual ~Bone();

   Bone& operator=(const Bone& bone);

   int id() const;
   void setId(int id);
   const Matrix& transform() const;
   const List<Weight>& weightList() const;
   Weight* weights() const;
   int count() const;

   void load(Stream* stream);
   void write(Stream* stream);

private:
   int mID;
   Matrix mInitTM;
   List<Weight> mWeights;
};
