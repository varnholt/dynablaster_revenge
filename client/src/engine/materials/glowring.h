// logo reflection

#ifndef GLOWRING_H
#define GLOWRING_H

#include "material.h"
#include "renderbuffer.h"

class GlowRing : public Material
{
public:

   // store normals, texture-coordinates and dynamic buffer of visible siluette edges
   class Buffer : public RenderBuffer
   {
   public:
      Buffer(Geometry *geo);
      unsigned int getNormalBuffer();
   private:
      unsigned int mNormal;
   };

   GlowRing(unsigned int col);
   void loadTexture(const char *filename);
   void load(Stream *stream);
   void add(Geometry *geo);
   virtual void renderAmbient();
   virtual void renderDiffuse();

private:
   int mAmount;
   int mShader;
   unsigned int mColor;
};

#endif
