#include "stdafx.h"
#include "logoreflection.h"
#include "stream.h"
#include "mesh.h"
#include "gldevice.h"
#include "renderbuffer.h"
#include "tga.h"
#include "uv.h"
#include "profiling.h"

LogoReflection::Buffer::Buffer(Geometry *geo) : RenderBuffer(geo)
{
   mSize=     geo->getIndexCount();
   mVertexCount= geo->getVertexCount();

   mVertex= createVertexBuffer(geo->getVertices(), mVertexCount*sizeof(Vector));
   mIndex=  createIndexBuffer(geo->getIndices(), mSize*sizeof(unsigned short));
   mNormal= createVertexBuffer(geo->getNormals(), mVertexCount*sizeof(Vector));
   mTexcoord= createVertexBuffer(geo->getUV(1), mVertexCount*sizeof(UV));

   geo->setVertexBuffer(mVertex);
}

unsigned int LogoReflection::Buffer::getNormalBuffer()
{ 
   return mNormal; 
}

unsigned int LogoReflection::Buffer::getTexcoordBuffer() 
{ 
   return mTexcoord; 
}

LogoReflection::LogoReflection(const char *map)
 : mAmount(0)
{
   int x,y;
   unsigned char *temp;

   loadtga(map, &temp, &x, &y);
   mColorMap= activeDevice->createTexture(temp, x,y, 1);
   free(temp);

   mShader= activeDevice->loadShader("logoreflection.vsh", "logoreflection.psh");
}

void LogoReflection::load(Stream *stream)
{
   int x,y;
   unsigned char *temp;

   loadDefault(stream);

/*
   loadtga("map", &temp, &x, &y);
   mColorMap= activeDevice->createTexture(temp, x,y, 1);
   free(temp);

   mShader= activeDevice->loadShader("logoreflection.vsh", "logoreflection.psh");
*/
}


void LogoReflection::add(Geometry *geo)
{
   Buffer *vb= new Buffer(geo);

   geo->setVertexBuffer(vb->getVertexBuffer());

   mVB.add(vb);
}


void LogoReflection::renderAmbient()
{
   int count;

   return;
}



void LogoReflection::renderDiffuse()
{
   int count;

   // set material parameters
   //   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);
   activeDevice->setCulling(false);

   // enable color texture
   glActiveTextureARB(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mColorMap);

   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   // enable vertex shader
   activeDevice->setShader(mShader);

   for (int i=0;i<mVB.size();i++)
   {
      // get vertex buffer
      Buffer *vb= (Buffer*)mVB[i];

      activeDevice->push(vb->getTransform());

      Vector viewer= vb->getCamera2Obj().translation();
//      activeDevice->setShaderConstant("camPos", viewer);

      // draw mesh
      glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb->getVertexBuffer() );
      glVertexPointer( 3, GL_FLOAT, 0, NULL );

      glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb->getTexcoordBuffer() );
      glTexCoordPointer( 2, GL_FLOAT, 0, NULL );  

      glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb->getNormalBuffer() );
      glNormalPointer(GL_FLOAT, 0, NULL );  

      glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, vb->getIndexBuffer() );
      glDrawElements( GL_TRIANGLES, vb->getSize(), GL_UNSIGNED_SHORT, NULL ); // render

      activeDevice->pop();
   }

   glDisable(GL_TEXTURE_2D);
   glDisableClientState(GL_VERTEX_ARRAY); // vertex data
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
}

