#include "spherefragment.h"

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
#include "image/image.h"
#include "vertex3d.h"

// cmath
#include <math.h>

// postproduction
#include "postproduction/blurfilter.h"

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

/*
      order= pos.y*3.0f + pos.x*2.0f;
*/

      mVertexCount += geo->getVertexCount();
      mIndexCount += geo->getIndexCount();
      mMatrix.add( mat );

      mRandom.add( (qrand() % 1000)*0.001 );
      mModelView.add( Matrix() );
      mFresnelFactors.add( 1.0f );
      mTime.add( order );
   }

   // create vertex and index buffer
   mVertexBuffer = activeDevice->createVertexBuffer( mVertexCount * sizeof(Vertex3D) );

   // fill vertex buffer
   volatile Vertex3D* vtx= (Vertex3D*)activeDevice->lockVertexBuffer(mVertexBuffer);

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
   volatile unsigned short* idx= (unsigned short*)activeDevice->lockIndexBuffer(mIndexBuffer);
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
      float t = fmod(time*2.5f + mTime[i], (float)M_PI * 4.0f);

      if (t > M_PI * 2.0f)
         t = (float)M_PI * 2.0f;

      float timeGrowth = sin(t + 1.5 * M_PI);
      float posScale = 0.1f * (1.0f + timeGrowth);

      posScale = 1.0f + mRandom[i] * posScale;

      // scale down during extension
      float size = 0.5f * (1.0f + timeGrowth);
      size = 1.0f - 0.2 * size;
      size *= size;

      // 0..1
      float amount = sin(t*0.5-M_PI*0.5)*0.5f+0.5f;

      // 0..2pi
      float r = amount*M_PI*2.0f;
      Matrix fragmentRotation = Matrix::scale(size,size,size) * Matrix::rotateX(r) * Matrix::rotateY(r);

      rot.translate(pos * posScale);

      mModelView[i]= fragmentRotation * rot * rotation;

      mFresnelFactors[i]= 1.0f - sin(t*0.5f);
//      mModelView[i]= mat * rotation;
   }
}


void SphereFragment::draw()
{
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);

   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertexBuffer );
   glVertexPointer( 3, GL_FLOAT, sizeof(Vertex3D), (GLvoid*)0 );
   glNormalPointer(GL_FLOAT, sizeof(Vertex3D), (GLvoid*)(3*sizeof(float)) );

   glClientActiveTexture(GL_TEXTURE0);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer( 4, GL_FLOAT, sizeof(Vertex3D), (GLvoid*)(6*sizeof(float)) );

   glClientActiveTexture(GL_TEXTURE1);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer( 3, GL_FLOAT, sizeof(Vertex3D), (GLvoid*)(10*sizeof(float)) );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, mIndexBuffer );
   glDrawElements( GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, 0 ); // render

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 );

   glClientActiveTexture(GL_TEXTURE1);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glClientActiveTexture(GL_TEXTURE0);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
}
