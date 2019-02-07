// gouraud shading

#ifndef GOURAUDSHADING_H
#define GOURAUDSHADING_H

#include "material.h"

class GouraudShading : public Material
{
public:
	void load(Stream *stream);
	void add(Geometry *geo);
	void render();

private:
   int mAmount;
   int mTexture;
   int mTexture2;
	int mShader;
};

#endif
