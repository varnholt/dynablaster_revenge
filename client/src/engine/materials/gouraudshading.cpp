// gouraud shading
// this material represents materialid= 0
// there is no texture, just color information

#include "stdafx.h"
#include "gouraudshading.h"
#include "stream.h"
#include "mesh.h"
#include "renderdevice.h"
#include "gldevice.h"
#include "renderbuffer.h"
#include "tga.h"
#include "uv.h"
#include "profiling.h"

class Buffer : public RenderBuffer
{
public:
   Buffer(Geometry *geo) : RenderBuffer(geo)
   {
      mNormal= createVertexBuffer(geo->getNormals(), mVertexCount*sizeof(Vector));
      mTexcoord= createVertexBuffer(geo->getUV(1), mVertexCount*sizeof(UV));
   }

   unsigned int getNormalBuffer() { return mNormal; }
   unsigned int getTexcoordBuffer() { return mTexcoord; }

private:
   unsigned int mNormal;
   unsigned int mTexcoord;
};

void GouraudShading::load(Stream *stream)
{
   int x,y;
   unsigned char *temp;

//   printf(" - gouraud shading\n");
   loadDefault(stream);

   loadtga("map", &temp, &x, &y);
   mTexture= activeDevice->createTexture(temp, x,y);
   free(temp);
}


void GouraudShading::add(Geometry *geo)
{
   Buffer *vb= new Buffer(geo);
   mVB.add(vb);
}



void GouraudShading::render()
{
   int count;

	glUseProgramObjectARB(0);

   glActiveTextureARB(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mTexture);

   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);

   activeDevice->setCulling( getCulling() );

   glColor4f(1,1,1,1);
   for (int i=0;i<mVB.size();i++)
   {
      // get vertex buffer
      Buffer *vb= (Buffer*)mVB[i];
      Geometry *geo= vb->getGeometry();

      // set transformation
      Matrix mat= vb->getTransform();
      activeDevice->push(vb->getTransform());

      // draw mesh
      glEnableClientState(GL_VERTEX_ARRAY); // vertex data
      glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb->getVertexBuffer() );
      glVertexPointer( 3, GL_FLOAT, 0, NULL );

      glEnableClientState(GL_NORMAL_ARRAY); // normal data
      glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb->getNormalBuffer() );
      glNormalPointer( GL_FLOAT, 0, NULL );  

      glClientActiveTextureARB(GL_TEXTURE0_ARB); // color map
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb->getTexcoordBuffer() );
      glTexCoordPointer( 2, GL_FLOAT, 0, NULL );  

      glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, vb->getIndexBuffer() );
      glDrawElements( GL_TRIANGLES, vb->getSize(), GL_UNSIGNED_SHORT, NULL ); // render


		glClientActiveTextureARB(GL_TEXTURE1_ARB); 
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glDisable(GL_TEXTURE_2D);
      glClientActiveTextureARB(GL_TEXTURE0_ARB); 
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glDisable(GL_TEXTURE_2D);
      glDisableClientState(GL_NORMAL_ARRAY);
      glDisableClientState(GL_VERTEX_ARRAY);

      activeDevice->pop();

   }

}

