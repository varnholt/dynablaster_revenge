// draws outlines

#ifndef OUTLINING_H
#define OUTLINING_H

#include "material.h"
#include "math/matrix.h"

class Outlining : public Material
{
public:
   Outlining(SceneGraph *scene);
   virtual ~Outlining();
   void load(Stream *stream);
   void add(Geometry *geo);
   void update(float, Node **nodes, const Matrix& cam);
   void renderDiffuse();

private:
   void init();
   void begin();
   void end();
   int calcEdgeIndices(unsigned int indexbuffer, Geometry *geo, const Vector& viewer);

   unsigned int mShader;

   Matrix mCamera;

};

#endif
