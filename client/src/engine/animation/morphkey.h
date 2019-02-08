// contains a morph-target for vertex-animations

#pragma once

#include "key.h"
#include "math/vector.h"
#include "tools/list.h"

class MorphKey : public KeyBase
{
public:
   MorphKey();

   const List<Vector>& getVertices() const;
   const List<Vector>& getNormals() const;

   int getVertexCount() const;

   virtual void load(Stream *stream);
   virtual void write(Stream *stream);

   void calculateNormals(const Array<unsigned short>& indexBuffer);

protected:
   List<Vector> mVertices;
   List<Vector> mNormals;
};

