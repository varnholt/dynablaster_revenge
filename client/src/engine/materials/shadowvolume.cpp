// shadow volumes

#include "stdafx.h"
#include "shadowvolume.h"
#include "stream.h"
#include "mesh.h"
#include "../render/renderdevice.h"
#include "gldevice.h"
#include "renderbuffer.h"
#include "tga.h"
#include "uv.h"
#include "profiling.h"

extern Vector globallight;
//const Vector globalLight= Vector(1,0,-1);//Vector(-0.4,1,0.2);

class ShadowBuffer : public RenderBuffer
{
public:
   ShadowBuffer(Geometry *geo) : RenderBuffer(geo)
   {
      mVertex=  createVertexBuffer(NULL, geo->getEdgeCount()*4*4*sizeof(float), false);
      mSize= 0;
   }
};


ShadowVolume::ShadowVolume()
{
   mTransform= activeDevice->loadShader("shadowvolume.vsh", "shadowvolume.psh");
}

void ShadowVolume::load(Stream *stream)
{
}


void ShadowVolume::add(Geometry *geo)
{
   ShadowBuffer *vb= new ShadowBuffer(geo);
   mVB.add(vb);
}


int ShadowVolume::calcEdgeIndices(unsigned int buffer, Geometry *geo, const Vector& light)
{
   float  t=0;
   Vector n1,n2;
   float  dir1, dir2;
   int    count;
   int    num=0;

   Vector *verts= geo->getVertices();

   //   double time= getCpuTick();

   int numtri= geo->getIndexCount()/3;

   Edge *edge= geo->getEdges();
   count= geo->getEdgeCount();

   volatile float *dst= (float*)activeDevice->lockVertexBuffer(buffer);

   for (int j=0;j<count;j++)
   {
      // vertices of two triangles with shared edge v1->v2
      const Vector& v1= verts[edge->i1];
      const Vector& v2= verts[edge->i2];
      const Vector& v3= verts[edge->i3];
      const Vector& v4= verts[edge->i4];

      // takes 8.5MI for 90.000 edges
      dir1= ((v2.y - v1.y)*(v3.z - v1.z) - (v2.z - v1.z)*(v3.y - v1.y)) * (light.x) +
            ((v2.z - v1.z)*(v3.x - v1.x) - (v2.x - v1.x)*(v3.z - v1.z)) * (light.y) +
            ((v2.x - v1.x)*(v3.y - v1.y) - (v2.y - v1.y)*(v3.x - v1.x)) * (light.z);

      dir2= ((v2.y - v1.y)*(v1.z - v4.z) - (v2.z - v1.z)*(v1.y - v4.y)) * (light.x) +
            ((v2.z - v1.z)*(v1.x - v4.x) - (v2.x - v1.x)*(v1.z - v4.z)) * (light.y) +
            ((v2.x - v1.x)*(v1.y - v4.y) - (v2.y - v1.y)*(v1.x - v4.x)) * (light.z);

      // draw edge if one poly facing to viewer, one face away
      if (dir1*dir2<0)
      {
         if (dir1<0) dir1=1000.0; else dir1=0;
         if (dir2<0) dir2=1000.0; else dir2=0;

         *dst++= v1.x;
         *dst++= v1.y;
         *dst++= v1.z;
         *dst++= dir1;

         *dst++= v1.x;
         *dst++= v1.y;
         *dst++= v1.z;
         *dst++= dir2;

         *dst++= v2.x;
         *dst++= v2.y;
         *dst++= v2.z;
         *dst++= dir2;

         *dst++= v2.x;
         *dst++= v2.y;
         *dst++= v2.z;
         *dst++= dir1;

         num+=4;
      }

      edge++;
   }

   activeDevice->unlockVertexBuffer(buffer);

   return num;
}


void ShadowVolume::update(Node **nodes)
{
//   Material::update(nodes);

   for (int i=0;i<mVB.size();i++)
   {
      // get vertex buffer
      ShadowBuffer *vb= (ShadowBuffer*)mVB[i];

      // inverse-transform camera position to local object space
      Vector light= vb->getWorld2Obj().get3x3() * -globallight;

      int count= calcEdgeIndices(vb->getVertexBuffer(), vb->getGeometry(), light);
      vb->setSize(count);
   }
}


void ShadowVolume::renderAmbient()
{
   // enable vertex shader
   activeDevice->setShader(mTransform);

   glEnable(GL_DEPTH_TEST);
   glColorMask(false, false, false, false);
   glDepthMask(false);

   glColorMask(true,true,true,true);
   glEnable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_SRC_COLOR);
   glColor4f(0.5,0,0,1);

   glDisable(GL_CULL_FACE);
   glEnable(GL_STENCIL_TEST);


   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data

   for (int i=0;i<mVB.size();i++)
   {
      // get vertex buffer
      ShadowBuffer *vb= (ShadowBuffer*)mVB[i];

      // set transformation
      const Matrix& mat= vb->getTransform();
      activeDevice->push(mat);

      // inverse-transform camera position to local object space
      Vector light= vb->getWorld2Obj().get3x3() * -globallight;

//      activeDevice->setShaderConstant("light", light);

      // draw mesh
      glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb->getVertexBuffer() );
      glVertexPointer( 4, GL_FLOAT, 0, NULL );

      glDrawArrays(GL_QUADS, 0, vb->getSize());

      activeDevice->pop();
   }

   glDisableClientState(GL_VERTEX_ARRAY); // vertex data

   glCullFace(GL_BACK);
   glColorMask(true,true,true,true);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();   
   glFrustum( -1.0f, 1.0f, -1.0f, 1.0f, 0.5f, 2.0f);

   glStencilFunc(GL_NOTEQUAL, 0x80, 0xff);
   glColor4f(164/255.0/2, 164/255.0/2, 164/255.0/2, 1.0);
//   glColor4f(1,1,1,1);
	glDisable(GL_DEPTH_TEST);
//   activeDevice->setShader(0);

//   glScalef(0,0,0);
   glBegin(GL_QUADS);
   glVertex4f(-2,-2,-1, 0);
   glVertex4f( 2,-2,-1, 0);
   glVertex4f( 2, 2,-1, 0);
   glVertex4f(-2, 2,-1, 0);
   glEnd();

   glStencilFunc(GL_ALWAYS, 0x0, 0xff);
   glDisable(GL_STENCIL_TEST);
   glDepthMask(true);
   glEnable(GL_DEPTH_TEST);

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();

   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

}

