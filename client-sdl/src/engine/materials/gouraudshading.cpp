// gouraud shading
// this material represents materialid= 0
// there is no texture, just color information

#include "gouraudshading.h"
#include "tools/stream.h"
#include "nodes/mesh.h"
#include "renderdevice.h"
#include "gldevice.h"
#include "render/renderbuffer.h"
#include "image/tga.h"
#include "render/uv.h"
#include "tools/profiling.h"

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
      glEnableVertexAttribArray(0); // vertex data
      glBindBufferARB( GL_ARRAY_BUFFER, vb->getVertexBuffer() );
      glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL  );

      glEnableVertexAttribArray(1); // normal data
      glBindBufferARB( GL_ARRAY_BUFFER, vb->getNormalBuffer() );
      glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL  );  

      glClientActiveTextureARB(GL_TEXTURE0_ARB); // color map
      glEnableVertexAttribArray(2);
      glBindBufferARB( GL_ARRAY_BUFFER, vb->getTexcoordBuffer() );
      glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, NULL  );  

      glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, vb->getIndexBuffer() );
      glDrawElements( GL_TRIANGLES, vb->getSize(), GL_UNSIGNED_SHORT, NULL ); // render


		glClientActiveTextureARB(GL_TEXTURE1_ARB); 
      glDisableVertexAttribArray(2);
      glDisable(GL_TEXTURE_2D);
      glClientActiveTextureARB(GL_TEXTURE0_ARB); 
      glDisableVertexAttribArray(2);
      glDisable(GL_TEXTURE_2D);
      glDisableVertexAttribArray(1);
      glDisableVertexAttribArray(0);

      activeDevice->pop();

   }

}

