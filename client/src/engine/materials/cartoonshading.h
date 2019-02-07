// cartoon shading

#ifndef CARTOONSHADING_H
#define CARTOONSHADING_H

#include "material.h"

class CartoonShading : public Material
{
public:
   void load(Stream *stream);
   void add(Geometry *geo);
   virtual void renderAmbient();
   virtual void renderDiffuse();

private:
   int mAmount;
   int mColorMap;
   int mGradientMap;
   int mShader;
	int mTransform;
};

#endif
