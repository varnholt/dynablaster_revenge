// cartoon shading implementation

#include "cartoonshading.h"
#include "tools/stream.h"
#include "nodes/mesh.h"
#include "renderdevice.h"
#include "gldevice.h"
#include "render/renderbuffer.h"
#include "image/tga.h"
#include "render/uv.h"
#include "tools/profiling.h"
#include "image.h"

Vector globallight= Vector(-2,2,1);

// pimped-up renderbuffer for cartoon shading
// store normals, texture-coordinates and dynamic buffer of visible siluette edges
class CartoonBuffer : public RenderBuffer
{
public:
   CartoonBuffer(Geometry *geo) : RenderBuffer(geo)
   {
      mSize=     geo->getIndexCount();
      mVertexCount= geo->getVertexCount();

      if (geo->getWeightCount()>0)
      {
         mOrgVerts= new Vector[geo->getVertexCount()];
         memcpy(mOrgVerts, geo->getVertices(), geo->getVertexCount()*sizeof(Vector));
         mWeights= geo->getWeights();
      }
      else
      {
         mOrgVerts= NULL;
         mWeights= NULL;
      }

      mVertex= createVertexBuffer(geo->getVertices(), mVertexCount*sizeof(Vector));
      mIndex=  createIndexBuffer(geo->getIndices(), mSize*sizeof(unsigned short));
      mNormal= createVertexBuffer(geo->getNormals(), mVertexCount*sizeof(Vector));
      mTexcoord= createVertexBuffer(geo->getUV(1), mVertexCount*sizeof(UV));
   }

   unsigned int getNormalBuffer() { return mNormal; }
   unsigned int getTexcoordBuffer() { return mTexcoord; }

private:
   unsigned int mNormal;
   unsigned int mTexcoord;
};

void CartoonShading::load(Stream *stream)
{
   int x,y;

//   printf(" - cartoon shading\n");
   loadDefault(stream);

   Image colormap("map");
   mColorMap= uploadMap(colormap, 1);

   Image gradient("gradient");
//   mGradientMap= uploadTexture1D(temp, x, 0);

   mShader= activeDevice->loadShader("cartoonshading.vsh", "cartoonshading.psh");
	mTransform= activeDevice->loadShader("transform.vsh", "");
}


void CartoonShading::add(Geometry *geo)
{
//   CartoonBuffer *vb= new CartoonBuffer(geo);
//   mVB.add(vb);
}


void CartoonShading::renderAmbient()
{
   int count;
/*
   return;

   // set material parameters
//   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);
   activeDevice->setCulling( getCulling() );

   // enable color texture
   glActiveTextureARB(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_1D);
   //   glDisable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_1D, mGradientMap);
   glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

   // enable required vertex arrays
   glEnableVertexAttribArray(0); // vertex data
   glEnableVertexAttribArray(1); // normal data

   // enable vertex shader
   activeDevice->setShader(mShader);

   // pass1: render all geometry using custom toon shader
//   glColor4f(1,1,1,1); // white
//   glColor3fv(mDiffuse.data());

   for (int i=0;i<mVB.size();i++)
   {
      // get vertex buffer
      CartoonBuffer *vb= (CartoonBuffer*)mVB[i];

      // set transformation
      const Matrix& mat= vb->getTransform();
      activeDevice->push(mat);

      // inverse-transform camera position to local object space
      Vector viewer= vb->getCamera2Obj().translation();

      Vector light= vb->getWorld2Obj().get3x3() * globallight;
      light.normalize();

//      activeDevice->setShaderConstant("light", light);
//      activeDevice->setShaderConstant("viewer", viewer);

      // draw mesh
      glBindBufferARB( GL_ARRAY_BUFFER, vb->getVertexBuffer() );
      glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL  );

      glBindBufferARB( GL_ARRAY_BUFFER, vb->getNormalBuffer() );
      glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL  );  

      glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, vb->getIndexBuffer() );
      glDrawElements( GL_TRIANGLES, vb->getSize(), GL_UNSIGNED_SHORT, NULL ); // render

      activeDevice->pop();
   }

   glDisable(GL_TEXTURE_1D);

   glDisableVertexAttribArray(0); // vertex data
   glDisableVertexAttribArray(1); // normal data
*/
}



void CartoonShading::renderDiffuse()
{
   int count;
/*
   // set material parameters
   //   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);
   activeDevice->setCulling( getCulling() );

   // enable color texture
   glActiveTextureARB(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mColorMap);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
   glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE); // multiply with underlying color
   glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 1);

   glDisable(GL_BLEND);
   glBlendFunc (GL_DST_COLOR, GL_SRC_COLOR);
//   glBlendFunc (GL_DST_COLOR, GL_ZERO);

   // enable required vertex arrays
   glEnableVertexAttribArray(0); // vertex data
   glEnableVertexAttribArray(2);

   // enable vertex shader
   activeDevice->setShader(mTransform);
//	activeDevice->setShader(0);

   // pass1: render all geometry using custom toon shader
   glColor4f(1,1,1,1); // white

   for (int i=0;i<mVB.size();i++)
   {
      // get vertex buffer
      CartoonBuffer *vb= (CartoonBuffer*)mVB[i];

      activeDevice->push(vb->getTransform());

      // draw mesh
      glBindBufferARB( GL_ARRAY_BUFFER, vb->getVertexBuffer() );
      glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL  );

      glBindBufferARB( GL_ARRAY_BUFFER, vb->getTexcoordBuffer() );
      glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, NULL  );  

      glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, vb->getIndexBuffer() );
      glDrawElements( GL_TRIANGLES, vb->getSize(), GL_UNSIGNED_SHORT, NULL ); // render

      activeDevice->pop();
   }

   glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 1);
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);
   glDisableVertexAttribArray(0); // vertex data
   glDisableVertexAttribArray(2);
*/
}

