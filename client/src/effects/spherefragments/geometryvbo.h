#ifndef VBOBASE_H
#define VBOBASE_H

// Qt
#include <QtOpenGL/QGLBuffer>
#include <QtOpenGL/QGLWidget>

// spherefragments
#include "vertex3d.h"

// forward declarations
class Geometry;

class GeometryVbo
{

public:

   GeometryVbo();
   virtual ~GeometryVbo();

   virtual void drawVbos(
      QGLBuffer* vertexBuffer,
      QGLBuffer* indexBuffer,
      int indicesCount
   );

   virtual void initialize();


protected:

   unsigned int mVertexBuffer;
   unsigned int mIndexBuffer;

   QVector<Vertex3D> mVertices;
   QVector<GLuint> mIndices;

   Geometry* mGeometry;
};

#endif // VBOBASE_H
