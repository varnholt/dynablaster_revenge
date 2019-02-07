#ifndef CLIPPER_H
#define CLIPPER_H

#include "vertex.h"
#include "tools/array.h"

class Clipper
{
public:
   enum Flags
   {
      Left = 1,
      Right = 2,
      Top = 4,
      Bottom = 8
   };

   Clipper(float left, float top, float right, float bottom);
   ~Clipper();

   void setBounds(float left, float top, float right, float bottom);

   Array<Vertex> clip(const Array<Vertex>& vertices);
   void enable();
   bool enable(const Array<Vertex>& sourceBounding);
   void disable();
   bool visible(const Array<Vertex>& vertices) const;
   float getWidth() const;
   float getHeight() const;

private:
   int getClipFlags(const Vertex& v) const;
   Array<Vertex> clipRight(const Array<Vertex>& vertices);
   Array<Vertex> clipLeft(const Array<Vertex>& vertices);
   Array<Vertex> clipTop(const Array<Vertex>& vertices);
   Array<Vertex> clipBottom(const Array<Vertex>& vertices);

   float mLeft;
   float mTop;
   float mRight;
   float mBottom;
   float mScreenWidth;
   float mScreenHeight;
   bool mEnabled;
};

#endif
