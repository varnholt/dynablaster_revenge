#include "earthbackground.h"

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

// cmath
#include <math.h>

// postproduction
#include "postproduction/blurfilter.h"


EarthBackground::EarthBackground()
  : mEarthShader(0),
    mAuraShader(0),

    mEarthTexture(),
    mNormalMapTexture(),

    mVertexBuffer(0),
    mIndexBuffer(0),
    mIndexCount(0),

    mLightParam(0),
    mCameraParam(0),
    mCloudMoveParam(0),

    mTextureMapParam(0),
    mNormalMapParam(0),
    mSpecularMapParam(0),
    mCloudMapParam(0)
{
   // init texture
   TexturePool* pool= TexturePool::Instance();

//   Image* image= new Image("earth_height");
//   image->buildNormalMap( 100 );  // strength of normal map
//   image->save("test.tga");

   mEarthTexture = pool->getTexture("earth_color");
   mNormalMapTexture = pool->getTexture("earth_normal");

   // create sphere
   int segVert= 128;
   int segHorz= 256;

   // create vertex and index buffer
   mVertexBuffer = activeDevice->createVertexBuffer( (segVert+1)*(segHorz+1) * sizeof(Vertex) );
   volatile Vertex* vtx= (Vertex*)activeDevice->lockVertexBuffer(mVertexBuffer);
   for (int sy=0; sy<=segVert; sy++)
   {
      float v= (float)sy / segVert;
      float y= cos( v * M_PI );
      float r= sin( v * M_PI );
      if (fabs(r)<=0.0001f)  r=0.0001f; // avoid singularity
      for (int sx=0; sx<=segHorz; sx++)
      {
         float u= (float)sx/segHorz;
         float x= cos( u * 2.0f * M_PI ) * r;
         float z= sin( u * 2.0f * M_PI ) * r;

         Vector tangent= Vector::normalize( Vector(-z,0.0f,x) );

         vtx->x= x;
         vtx->y= y;
         vtx->z= z;
         vtx->tx= tangent.x;
         vtx->ty= tangent.y;
         vtx->tz= tangent.z;
         vtx->u= u;
         vtx->v= v;
         vtx++;
      }
   }
   activeDevice->unlockVertexBuffer(mVertexBuffer);

   // create index buffer
   mIndexCount= segHorz*segVert*6;
   mIndexBuffer= activeDevice->createIndexBuffer( mIndexCount * sizeof(unsigned short) );
   volatile unsigned short* idx= (unsigned short*)activeDevice->lockIndexBuffer(mIndexBuffer);
   for (int sy=0; sy<segVert; sy++)
   {
      int r1= sy*(segHorz+1);
      int r2= (sy+1)*(segHorz+1);
      for (int sx=0; sx<segHorz; sx++)
      {
         *idx++ = r1+sx;
         *idx++ = r1+sx+1;
         *idx++ = r2+sx;

         *idx++ = r1+sx+1;
         *idx++ = r2+sx+1;
         *idx++ = r2+sx;
      }
   }
   activeDevice->unlockIndexBuffer(mIndexBuffer);
}


void EarthBackground::begin()
{
   if (!mEarthShader)
   {
      // init shader
      mEarthShader = activeDevice->loadShader(
         "earth-vert.glsl",
         "earth-frag.glsl"
      );

      mLightParam = activeDevice->getParameterIndex("osLightPos");
      mCameraParam = activeDevice->getParameterIndex("osCameraPos");
      mCloudMoveParam = activeDevice->getParameterIndex("moveCloud");

      mTextureMapParam = activeDevice->getParameterIndex("texturemap");
      mNormalMapParam = activeDevice->getParameterIndex("normalmap");
      mSpecularMapParam = activeDevice->getParameterIndex("specularmap");
      mCloudMapParam = activeDevice->getParameterIndex("cloudmap");

      mAuraShader = activeDevice->loadShader(
        "earthaura-vert.glsl",
        "earthaura-frag.glsl"
     );
   }

   // activate shader
   activeDevice->setShader(mEarthShader);

   glActiveTexture(GL_TEXTURE0_ARB);
   glBindTexture(GL_TEXTURE_2D, mEarthTexture);
   activeDevice->bindSampler(mTextureMapParam, 0);

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mNormalMapTexture);
   activeDevice->bindSampler(mNormalMapParam, 1);
}


void EarthBackground::animate(float)
{
}

void EarthBackground::drawEarth(const Vector& camPos)
{
   begin();

   Matrix mv;
   glGetFloatv(GL_MODELVIEW_MATRIX, mv.data());
   Matrix invModel= mv.invert();

   float t= GlobalTime::Instance()->getTime();
//   Vector lightPosition = Vector(cos(t) * 30, -20, sin(t) * 30);
//   Vector lightPosition = Vector(0, 0, -50);
   Vector lightPosition = Vector(20, -20, -1);

   glColor4f(1,1,1,1);
   activeDevice->setParameter(
      mLightParam,
      invModel * lightPosition
   );

   activeDevice->setParameter(
      mCameraParam,
      invModel * camPos
   );

   activeDevice->setParameter(
      mCloudMoveParam,
      Vector(t*0.001f, 0, 0)
   );

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertexBuffer );
   glVertexPointer( 3, GL_FLOAT, sizeof(Vertex), (GLvoid*)0 );
   glNormalPointer(GL_FLOAT, sizeof(Vertex), (GLvoid*)(3*sizeof(float)) );
   glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), (GLvoid*)(6*sizeof(float)) );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, mIndexBuffer );
   glDrawElements( GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, 0 ); // render

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 );

   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);

   end();
}

float p= 0.0f;
void EarthBackground::draw(const Vector& camPos)
{
   drawEarth( camPos );

   activeDevice->setShader(mAuraShader);

   glLoadIdentity();
   glRotatef(-12.0, 1,0,0);
   glRotatef(-1.0, 0,1,0);


   float z= 0.50;// + sin(p) * 0.1;
   p+=0.001f;
   // qDebug("z: %f ", z);


   const Vector color(0.7f, 0.9f, 1.5f);

   const int segments= 128;
   const float r1= (float)sqrt(1.0-z*z);
   const float r2= r1+ 0.05f;
   float a= 0.0f;
   float da= (float) (M_PI * 2.0f / segments);


//   mCamera= Vector(-0.15f, 0.45f, 2.0f);


   glEnable(GL_BLEND);
//   glDisable(GL_CULL_FACE);
   glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
   glBegin(GL_TRIANGLE_STRIP);
   for (int i=0; i<=segments; i++)
   {
      float x= sin(a);
      float y= cos(a);
      a+=da;

      glTexCoord4f(0.0f, 0.0f, 0.0f, 0.0f);
      glVertex3f(x*r2, y*r2, z);

      glTexCoord4f(color.x, color.y, color.z, 1.0f);
      glVertex3f(x*r1, y*r1, z);
   }
   glEnd();
   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//   glEnable(GL_CULL_FACE);

   activeDevice->setShader(0);
}



void EarthBackground::end()
{
   activeDevice->setShader(0);

   glActiveTexture(GL_TEXTURE1_ARB);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0_ARB);
   glBindTexture(GL_TEXTURE_2D, 0);

}

