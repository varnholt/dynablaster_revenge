#include "clipper.h"
#include "framework/gldevice.h"
#include "framework/framebuffer.h"
#include <math.h>

Clipper::Clipper(float left, float top, float right, float bottom)
: mLeft(left)
, mTop(top)
, mRight(right)
, mBottom(bottom)
, mScreenWidth(1920)
, mScreenHeight(1080)
, mEnabled(false)
{
}

Clipper::~Clipper()
{
}

float Clipper::getWidth() const
{
   return mRight - mLeft;
}

float Clipper::getHeight() const
{
   return mBottom - mTop;
}

void Clipper::setBounds(float left, float top, float right, float bottom)
{
   mLeft = left;
   mTop = top;
   mRight = right;
   mBottom = bottom;
}

// perform clip test
int Clipper::getClipFlags(const Vertex& v) const
{
   int clip= 0;

   if (v.x < mLeft) clip |= Left;
   if (v.x > mRight) clip |= Right;
   if (v.y < mTop) clip |= Top;
   if (v.y > mBottom) clip |= Bottom;

   return clip;
}

Array<Vertex> Clipper::clipLeft(const Array<Vertex>& vertices)
{
   Array<Vertex> res;
   int size= vertices.size();

   if (size<=0)
      return res;

   int c1= getClipFlags(vertices[size-1]) & Left;
   Vertex* v1= &vertices[size-1];

   for (int i=0;i<size;i++)
   {
      int c2= getClipFlags(vertices[i]) & Left;
      Vertex* v2= &vertices[i];

      if (!c1)
         res.add(*v1);

      if (c1 != c2)
      {
          // v1.x + (v2.x - v1.x)*t = mLeft
          float t = (mLeft - v1->x) / (v2->x - v1->x);
          Vertex v= *v1 + (*v2 - *v1)*t;
          res.add( v );
      }

      v1= v2;
      c1= c2;
   }

   return res;
}

Array<Vertex> Clipper::clipRight(const Array<Vertex>& vertices)
{
   Array<Vertex> res;
   int size= vertices.size();

   if (size<=0)
      return res;

   int c1= getClipFlags(vertices[size-1]) & Right;
   Vertex* v1= &vertices[size-1];

   for (int i=0;i<size;i++)
   {
      int c2= getClipFlags(vertices[i]) & Right;
      Vertex* v2= &vertices[i];

      if (!c1)
         res.add(*v1);

      if (c1 != c2)
      {
         // v1.x + (v2.x - v1.x)*t = mRight
         float t = (mRight - v1->x) / (v2->x - v1->x);
         Vertex v= *v1 + (*v2 - *v1)*t;
         res.add( v );
      }

      v1= v2;
      c1= c2;
   }

   return res;
}


Array<Vertex> Clipper::clipTop(const Array<Vertex>& vertices)
{
   Array<Vertex> res;
   int size= vertices.size();

   if (size<=0)
      return res;

   int c1= getClipFlags(vertices[size-1]) & Top;
   Vertex* v1= &vertices[size-1];

   for (int i=0;i<size;i++)
   {
      int c2= getClipFlags(vertices[i]) & Top;
      Vertex* v2= &vertices[i];

      if (!c1)
         res.add(*v1);

      if (c1 != c2)
      {
         // v1.y + (v2.y - v1.y)*t = mTop
         float t = (mTop - v1->y) / (v2->y - v1->y);
         Vertex v= *v1 + (*v2 - *v1)*t;
         res.add( v );
      }

      v1= v2;
      c1= c2;
   }

   return res;
}


Array<Vertex> Clipper::clipBottom(const Array<Vertex>& vertices)
{
   Array<Vertex> res;
   int size= vertices.size();

   if (size<=0)
      return res;

   int c1= getClipFlags(vertices[size-1]) & Bottom;
   Vertex* v1= &vertices[size-1];

   for (int i=0;i<size;i++)
   {
      int c2= getClipFlags(vertices[i]) & Bottom;
      Vertex* v2= &vertices[i];

      if (!c1)
         res.add(*v1);

      if (c1 != c2)
      {
         // v1.y + (v2.y - v1.y)*t = mBottom
         float t = (mBottom - v1->y) / (v2->y - v1->y);
         Vertex v= *v1 + (*v2 - *v1)*t;
         res.add( v );
      }

      v1= v2;
      c1= c2;
   }

   return res;
}

Array<Vertex> Clipper::clip(const Array<Vertex>& vertices)
{
   Array<Vertex> left= clipLeft(vertices);
   Array<Vertex> right= clipRight(left);
   Array<Vertex> top= clipTop(right);
   Array<Vertex> bottom= clipBottom(top);

   return bottom;
}

bool Clipper::visible(const Array<Vertex>& vertices) const
{
   int flags= -1;
   int size= vertices.size();
   for (int i=0;i<size;i++)
   {
      flags &= getClipFlags(vertices[i]);
   }
   return (flags == 0);
}

bool Clipper::enable(const Array<Vertex>& vertices)
{
   int allflags= 0;
   int anyflags= Left | Right | Top | Bottom;
   int size= vertices.size();
   for (int i=0;i<size;i++)
   {
      const Vertex& v= vertices[i];
      int flag= getClipFlags(v);
      allflags |= flag;
      anyflags &= flag;
   }

   // some flag is set for all vertices: the polygon is completely outside the clip area
   if (anyflags != 0)
      return false;

   // at least one vertex is outside the clip area: enable clipping
   if (allflags != 0)
      enable();

   return true;
}

void Clipper::enable()
{
   if (!mEnabled)
   {
      mEnabled= true;

      int width, height;
      FrameBuffer *fb= FrameBuffer::Instance();
      if (fb)
      {
         width= fb->width();
         height= fb->height();
      }
      else
      {
         width= activeDevice->getWidth();
         height= activeDevice->getHeight();
      }

      glScissor(
            floor(mLeft * width / mScreenWidth),
            floor((mScreenHeight-mBottom-1) * height / mScreenHeight),
            ceil((mRight-mLeft) * width / mScreenWidth)+1,
            ceil((mBottom-mTop) * height / mScreenHeight)+1
      );
      glEnable(GL_SCISSOR_TEST);
   }
}

void Clipper::disable()
{
   if (mEnabled)
   {
      mEnabled= false;
      glDisable(GL_SCISSOR_TEST);
      glScissor(
            0,
            0,
            activeDevice->getWidth(),
            activeDevice->getHeight()
      );
   }
}

