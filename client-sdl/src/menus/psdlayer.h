#pragma once

#include "image/psd.h"
#include "render/texture.h"
#include "tools/array.h"

class Image;

/// \brief GLES3 port of client/src/menus/psdlayer.cpp.
///
/// renderScaled(), getVertices() and setColor()/mRed/mGreen/mBlue are dropped - none of them
/// have any real caller in menus/ (setColor() is declared and defined but never invoked, so the
/// RGB tint it would have set is always the default white; renderScaled() and getVertices() are
/// declared/defined but likewise never called). The fixed-function per-vertex color this used to
/// carry (glColor4f) is now just the "alpha" uniform on the shared texalphaignore shader.
class PSDLayer
{
public:
   struct Vertex
   {
      Vertex() : x(0.0f), y(0.0f), z(0.0f), u(0.0f), v(0.0f) {};
      Vertex(float x, float y, float z, float u, float v) : x(x), y(y), z(z), u(u), v(v) {};

      float x, y, z;
      float u, v;
   };

   PSDLayer();
   PSDLayer(PSD::Layer* layer, float z = -1.0f, bool unwrap = true);
   virtual ~PSDLayer();

   PSD::Layer* getLayer() const;
   unsigned int getTexture() const;
   unsigned int getVertexBuffer() const;
   unsigned int getIndexBuffer() const;
   float getOpacity() const;
   void setOpacity(float opacity);

   void render(float x = 0.0f, float y = 0.0f, float alpha = 1.0f);

   float getU() const;
   float getV() const;

   int getWidth() const;
   int getHeight() const;
   int getLeft() const;
   int getRight() const;
   int getTop() const;
   int getBottom() const;

private:
   PSD::Layer* mLayer;
   Texture mTexture;
   unsigned int mVertexBuffer;
   unsigned int mIndexBuffer;
   float mOpacity;
   float mU;
   float mV;
};
