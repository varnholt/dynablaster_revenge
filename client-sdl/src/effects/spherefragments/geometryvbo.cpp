#include "geometryvbo.h"

// engine
#include "gldevice.h"
#include "math/vector.h"
#include "render/geometry.h"
#include "render/uv.h"

GeometryVbo::GeometryVbo() : mVertexBuffer(0), mIndexBuffer(0), mGeometry(0)
{
}

GeometryVbo::~GeometryVbo()
{
   // TODO: delete buffers
}

void GeometryVbo::initialize()
{
   // create vertex and index buffer
   mVertexBuffer = activeDevice->createVertexBuffer(mGeometry->getVertexCount() * sizeof(Vertex3D));
   mIndexBuffer = activeDevice->createIndexBuffer(mGeometry->getIndexCount() * sizeof(unsigned short));

   Vector* vertices = mGeometry->getVertices();
   Vector* normals = mGeometry->getNormals();
   UV* uvs = mGeometry->getUV(1);
   unsigned short* indices = mGeometry->getIndices();

   // fill vertex buffer
   Vertex3D* vtx = (Vertex3D*)activeDevice->lockVertexBuffer(mVertexBuffer);
   for (int i = 0; i < mGeometry->getVertexCount(); i++)
   {
      vtx[i].mPosition = vertices[i];
      vtx[i].mNormal = normals[i];
      vtx[i].mU = uvs[i].u;
      vtx[i].mV = uvs[i].v;
      vtx[i].mIndex = 0.0f;
   }
   activeDevice->unlockVertexBuffer(mVertexBuffer);

   // fill index buffer
   unsigned short* idx = (unsigned short*)activeDevice->lockIndexBuffer(mIndexBuffer);
   for (int i = 0; i < mGeometry->getIndexCount(); i++)
   {
      *idx++ = indices[i];
   }
   activeDevice->unlockIndexBuffer(mIndexBuffer);
}
