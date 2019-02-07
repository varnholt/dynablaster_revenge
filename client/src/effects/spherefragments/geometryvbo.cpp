#include "geometryvbo.h"

// Qt
#include <QGLBuffer>
#include <QGLShaderProgram>

// engine
#include "framework/globaltime.h"
#include "gldevice.h"
#include "nodes/mesh.h"
#include "renderdevice.h"
#include "render/texturepool.h"
#include "nodes/scenegraph.h"
#include "tools/filestream.h"
#include "math/vector.h"

// cmath
#include <math.h>


GeometryVbo::GeometryVbo()
 : mVertexBuffer(0),
   mIndexBuffer(0)
{
}


GeometryVbo::~GeometryVbo()
{
   // TODO: delete buffers
}


void GeometryVbo::drawVbos(
   QGLBuffer* /*vertexBuffer*/,
   QGLBuffer* /*indexBuffer*/,
   int indicesCount
)
{
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, mVertexBuffer );
   glVertexPointer( 3, GL_FLOAT, sizeof(Vertex3D), (GLvoid*)0 );
   glNormalPointer(GL_FLOAT, sizeof(Vertex3D), (GLvoid*)sizeof(Vector) );
   glTexCoordPointer( 3, GL_FLOAT, sizeof(Vertex3D), (GLvoid*)(sizeof(Vector)*2) );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, mIndexBuffer );
   glDrawElements( GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, 0 ); // render

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}



void GeometryVbo::initialize()
{
   // create vertex and index buffer
   mVertexBuffer= activeDevice->createVertexBuffer( mGeometry->getVertexCount() * sizeof(Vertex3D) );
   mIndexBuffer= activeDevice->createIndexBuffer( mGeometry->getIndexCount() * sizeof(unsigned short) );

   Vector* vertices = mGeometry->getVertices();
   Vector* normals = mGeometry->getNormals();
   UV* uvs = mGeometry->getUV(1);
   unsigned short* indices= mGeometry->getIndices();

   // fill vertex buffer
   Vertex3D* vtx= (Vertex3D*)activeDevice->lockVertexBuffer( mVertexBuffer );
   for (int i=0; i<mGeometry->getVertexCount(); i++)
   {
      vtx[i].mPosition= vertices[i];
      vtx[i].mNormal= normals[i];
      vtx[i].mU= uvs[i].u;
      vtx[i].mV= uvs[i].v;
      vtx[i].mIndex= 0.0f;
   }
   activeDevice->unlockVertexBuffer(mVertexBuffer);

   // fill vertex buffer
   unsigned short* idx= (unsigned short*)activeDevice->lockIndexBuffer( mIndexBuffer );
   for (int i=0; i<mGeometry->getIndexCount(); i++)
   {
      *idx++= indices[i];
   }
   activeDevice->unlockIndexBuffer(mIndexBuffer);
}
