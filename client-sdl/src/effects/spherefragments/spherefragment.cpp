#include "spherefragment.h"

// engine
#include "gldevice.h"
#include "nodes/mesh.h"
#include "render/geometry.h"
#include "render/uv.h"
#include "math/vector.h"
#include "image/image.h"
#include "tools/random.h"
#include "vertex3d.h"

// cmath
#include <math.h>

namespace
{
// MSVC only defines M_PI when _USE_MATH_DEFINES is set before every <math.h> include site
// (fragile project-wide), so this is a self-contained local constant instead - same pattern
// already established in menus/menupagefadeanimation.cpp.
constexpr float kPi = 3.14159265358979323846f;
}

SphereFragment::SphereFragment(const Array<Mesh*>& meshList, Image* orderImage)
 : mVertexBuffer(0),
   mIndexBuffer(0)
{
   mMatrix.init(meshList.size());

   mVertexCount= 0;
   mIndexCount= 0;

   for (int mIndex=0; mIndex < meshList.size(); mIndex++)
   {
      Mesh* mesh= meshList[mIndex];
      const Matrix& mat= mesh->getTransform();
      Geometry* geo= mesh->getPart(0);

      float order= 0.0f;

      Vector pos= mat.translation();

      Vector p= pos;
      p.normalize();
      p += Vector(0,0,1);

      float m= 0.5f / sqrt(p*p);
      float u= 0.5f + p.x*m;
      float v= 0.5f + p.y*m;

      unsigned int rgb= orderImage->getPixel(u, v);
      order= ((rgb >> 16 & 255) + (rgb >> 8 & 255) + (rgb & 255)) / 96.0f;

      mVertexCount += geo->getVertexCount();
      mIndexCount += geo->getIndexCount();
      mMatrix.add( mat );

      mRandom.add( frand(1.0f) );
      mModelView.add( Matrix() );
      mFresnelFactors.add( 1.0f );
      mTime.add( order );
   }

   // create vertex and index buffer
   mVertexBuffer = activeDevice->createVertexBuffer( mVertexCount * sizeof(Vertex3D) );

   // fill vertex buffer
   Vertex3D* vtx= (Vertex3D*)activeDevice->lockVertexBuffer(mVertexBuffer);

   for (int m=0; m<meshList.size(); m++)
   {
      Mesh* mesh= meshList[m];
      const Matrix& mat= mesh->getTransform();
      Matrix norm= mat.get3x3();
      Geometry* geo= mesh->getPart(0);
      Vector* vertices= geo->getVertices();
      Vector* normals= geo->getNormals();
      UV* uvs= geo->getUV(1);

      for (int v=0; v<geo->getVertexCount(); v++)
      {
         const Vector& p= vertices[v];
         const Vector& n= normals[v];

         Vector surface= mat * p;
         Vector nrm= norm * n;
         surface.normalize();

         float orientation= (nrm * surface);
         if (orientation < 0.0f)
            orientation= 0.0f;

         vtx->mPosition.x= p.x;
         vtx->mPosition.y= p.y;
         vtx->mPosition.z= p.z;
         vtx->mNormal.x= n.x;
         vtx->mNormal.y= n.y;
         vtx->mNormal.z= n.z;
         vtx->mU= uvs[v].u;
         vtx->mV= uvs[v].v;
         vtx->mIndex= (float)m;
         vtx->mBlend= orientation*orientation;
         vtx->mTangent.x= -n.y;
         vtx->mTangent.y= n.x;
         vtx->mTangent.z= 0.0f;
         vtx++;
      }
   }

   activeDevice->unlockVertexBuffer(mVertexBuffer);

   // fill index buffer
   mIndexBuffer = activeDevice->createIndexBuffer( mIndexCount * sizeof(unsigned short) );
   unsigned short* idx= (unsigned short*)activeDevice->lockIndexBuffer(mIndexBuffer);
   unsigned short offset= 0;
   for (int i=0; i<meshList.size(); i++)
   {
      Mesh* mesh= meshList[i];
      Geometry* geo= mesh->getPart(0);
      unsigned short* indices= geo->getIndices();
      for (int v=0; v<geo->getIndexCount(); v++)
      {
         unsigned short index= indices[v];
         *idx++= offset + index;
      }
      offset += geo->getVertexCount();
   }
   activeDevice->unlockIndexBuffer(mIndexBuffer);
}


int SphereFragment::getPartCount() const
{
   return mMatrix.size();
}


Matrix* SphereFragment::getMatrices() const
{
   return mModelView.data();
}


float* SphereFragment::getFresnelFactors() const
{
   return mFresnelFactors.data();
}


void SphereFragment::animate(float time, const Matrix& rotation)
{
   for (int i = 0; i<mMatrix.size(); i++)
   {
      const Matrix& mat = mMatrix[i];
      Vector pos = mat.translation();
      Matrix rot = mat.get3x3();

      // 4 * PI
      // => one loop of action
      // => another loop of idle
      float t = fmodf(time*2.5f + mTime[i], kPi * 4.0f);

      if (t > kPi * 2.0f)
         t = kPi * 2.0f;

      float timeGrowth = sin(t + 1.5f * kPi);
      float posScale = 0.1f * (1.0f + timeGrowth);

      posScale = 1.0f + mRandom[i] * posScale;

      // scale down during extension
      float size = 0.5f * (1.0f + timeGrowth);
      size = 1.0f - 0.2f * size;
      size *= size;

      // 0..1
      float amount = sin(t*0.5f-kPi*0.5f)*0.5f+0.5f;

      // 0..2pi
      float r = amount*kPi*2.0f;
      Matrix fragmentRotation = Matrix::scale(size,size,size) * Matrix::rotateX(r) * Matrix::rotateY(r);

      rot.translate(pos * posScale);

      mModelView[i]= fragmentRotation * rot * rotation;

      mFresnelFactors[i]= 1.0f - sin(t*0.5f);
   }
}


void SphereFragment::draw()
{
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glEnableVertexAttribArray(2);
   glEnableVertexAttribArray(3);

   glBindBuffer( GL_ARRAY_BUFFER, mVertexBuffer );
   glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)0 );
   glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)(3*sizeof(float)) );
   glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)(6*sizeof(float)) );
   glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)(10*sizeof(float)) );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer );
   glDrawElements( GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, 0 ); // render

   glDisableVertexAttribArray(3);
   glDisableVertexAttribArray(2);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(0);
}
