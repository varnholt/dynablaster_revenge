#pragma once

#include "image/psd.h"
#include "tools/array.h"
#include "render/texture.h"

class Image;

class PSDLayer
{
public:
   struct Vertex
   {
      Vertex() : x(0.0f), y(0.0f), z(0.0f), u(0.0f), v(0.0f) {};
      Vertex(float x, float y, float z, float u, float v) : x(x), y(y), z(z), u(u), v(v) {};

      float x,y,z;
      float u,v;
   };

   PSDLayer();
   PSDLayer(PSD::Layer* layer, float z=-1.0f, bool unwrap= true);
   virtual ~PSDLayer();

   PSD::Layer*  getLayer() const;
   unsigned int getTexture() const;
   unsigned int getVertexBuffer() const;
   unsigned int getIndexBuffer() const;
   float getOpacity() const;
   void setOpacity(float opacity);
   void setColor(float r, float g, float b);

   void render(float x=0.0f, float y=0.0f, float alpha=1.0f);

   void renderScaled(
      float x=0.0f,
      float y=0.0f,
      float alpha=1.0f,
      float scaleX = 1.0f,
      float scaleY = 1.0f,
      float scaleZ = 1.0f
   );

   float getU() const;
   float getV() const;

   int getWidth() const;
   int getHeight() const;
   int getLeft() const;
   int getRight() const;
   int getTop() const;
   int getBottom() const;

   const Array<Vertex>& getVertices() const;

private:
   int log2(int x) const;

   PSD::Layer*  mLayer;
   Texture      mTexture;
   unsigned int mVertexBuffer;
   unsigned int mIndexBuffer;
   float        mRed,mGreen,mBlue;
   float        mOpacity;
   float        mU;
   float        mV;
};
