// logo reflection

#ifndef LOGOREFLECTION_H
#define LOGOREFLECTION_H

#include "material.h"
#include "renderbuffer.h"

class LogoReflection : public Material
{
public:

   // store normals, texture-coordinates and dynamic buffer of visible siluette edges
   class Buffer : public RenderBuffer
   {
   public:
      Buffer(Geometry *geo);
      unsigned int getNormalBuffer();
      unsigned int getTexcoordBuffer();
   private:
      unsigned int mNormal;
      unsigned int mTexcoord;
   };

   LogoReflection(const char *map);
   void loadTexture(const char *filename);
   void load(Stream *stream);
   void add(Geometry *geo);
   virtual void renderAmbient();
   virtual void renderDiffuse();

private:
   int mAmount;
   int mColorMap;
   int mShader;
};

#endif
