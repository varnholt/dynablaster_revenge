#pragma once

class Node;
class Vector;
class Geometry;
class Weight;
class Matrix;

class RenderBuffer
{
public:
   RenderBuffer(Geometry *geo);
   ~RenderBuffer();

   void          update(Node **nodelist); // pimp-up teh parametathinggy.
   const Matrix& getTransform() const;    // get transformation matrix of mesh
   int           getSize() const;         // number of indices in index buffer (triangles*3)
   unsigned int  getVertexBuffer() const; // get vertex buffer (handle to opengl)
   unsigned int  getIndexBuffer() const;  // get index buffer (handle to opengl)
   Geometry*     getGeometry() const;     // it's possible but just don't do it. it's evil and ugly
   void          setSize(int num);
   bool          getVisible() const;

   virtual void  clear();

protected:
   Node           *mNode;
   Geometry       *mGeometry;
   unsigned int   mFormat;
   int            mSize;
   int            mVertexCount;
   Vector         *mOrgVerts;
   Weight         *mWeights;

// vtx/idx-buffer handles
   unsigned int   mVertex;
   unsigned int   mIndex;

protected:
   unsigned int  createVertexBuffer(void *data, int size, bool dyn= false);
   unsigned int  createIndexBuffer(void *data, int size, bool dyn= false);
   void deleteBuffer(unsigned int buffer);

};
