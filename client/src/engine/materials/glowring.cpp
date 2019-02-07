#include "stdafx.h"
#include "glowring.h"
#include "stream.h"
#include "mesh.h"
#include "gldevice.h"
#include "renderbuffer.h"
#include "tga.h"
#include "uv.h"
#include "profiling.h"

GlowRing::Buffer::Buffer(Geometry *geo) : RenderBuffer(geo)
{
   mSize=     geo->getIndexCount();
   mVertexCount= geo->getVertexCount();

   mVertex= createVertexBuffer(geo->getVertices(), mVertexCount*sizeof(Vector));
   mIndex=  createIndexBuffer(geo->getIndices(), mSize*sizeof(unsigned short));
   mNormal= createVertexBuffer(geo->getNormals(), mVertexCount*sizeof(Vector));

   geo->setVertexBuffer(mVertex);
}

unsigned int GlowRing::Buffer::getNormalBuffer()
{ 
   return mNormal; 
}

GlowRing::GlowRing(unsigned int col)
 : mAmount(0)
{
   mColor= col;
   mShader= activeDevice->loadShader("glowring.vsh", "glowring.psh");
}

void GlowRing::load(Stream *stream)
{
   int x,y;
   unsigned char *temp;

   loadDefault(stream);

   /*
   loadtga("map", &temp, &x, &y);
   mColorMap= activeDevice->uploadTexture(temp, x,y, 1);
   free(temp);

   mShader= activeDevice->loadShader("logoreflection.vsh", "logoreflection.psh");
   */
}


void GlowRing::add(Geometry *geo)
{
   Buffer *vb= new Buffer(geo);

   geo->setVertexBuffer(vb->getVertexBuffer());

   mVB.add(vb);
}


void GlowRing::renderAmbient()
{
   int count;

   return;
}



void GlowRing::renderDiffuse()
{
   int count;

   // set material parameters
   //   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);
   activeDevice->setCulling(false);

   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data
   glEnableClientState(GL_NORMAL_ARRAY);

   glColor4f((mColor>>16&255)/255.0f, (mColor>>8&255)/255.0f, (mColor&255)/255.0f, 1.0f);
   // enable vertex shader
   activeDevice->setShader(mShader);

   for (int i=0;i<mVB.size();i++)
   {
      // get vertex buffer
      Buffer *vb= (Buffer*)mVB[i];

      activeDevice->push(vb->getTransform());

      // draw mesh
      glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb->getVertexBuffer() );
      glVertexPointer( 3, GL_FLOAT, 0, NULL );

      glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb->getNormalBuffer() );
      glNormalPointer(GL_FLOAT, 0, NULL );  

      glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, vb->getIndexBuffer() );
      glDrawElements( GL_TRIANGLES, vb->getSize(), GL_UNSIGNED_SHORT, NULL ); // render

      activeDevice->pop();
   }

   glDisable(GL_TEXTURE_2D);
   glDisableClientState(GL_VERTEX_ARRAY); // vertex data
   glDisableClientState(GL_NORMAL_ARRAY);
}

