#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

class Geometry;
class Matrix;
class Node;

class VertexBuffer
{
public:
   VertexBuffer(Geometry *geo);
   ~VertexBuffer();
   
   void          update(Node **nodelist);

   Geometry*     getGeometry() const;
   unsigned int  getVertexBuffer() const;
   unsigned int  getIndexBuffer() const;
   int           getIndexCount() const;
   void          setIndexCount(int count);
   void          setIndexBuffer( unsigned short* indices, int count );

private:
   Geometry       *mGeometry;
   unsigned int   mVertex;
   unsigned int   mIndex;
   int            mSize;
};

#endif
