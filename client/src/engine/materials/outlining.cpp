// cartoon shading implementation

#include "stdafx.h"
#include "outlining.h"
#include "tools/stream.h"
#include "nodes/mesh.h"
#include "renderdevice.h"
#include "gldevice.h"
#include "render/renderbuffer.h"
#include "render/uv.h"
#include "tools/profiling.h"
#include "render/vertexbuffer.h"

Outlining::Outlining(SceneGraph *scene)
: Material(scene, -4),
  mShader(0)
{
}


Outlining::~Outlining()
{
}


void Outlining::init()
{
   mShader= activeDevice->loadShader("outlining-vert.glsl", "outlining-frag.glsl");
}

void Outlining::load(Stream*)
{
}

void Outlining::add(Geometry *geo)
{
   VertexBuffer *vb= mPool->add(geo);

   activeDevice->allocateVertexBuffer( vb->getVertexBuffer(), sizeof(Vector)*geo->getVertexCount() );
   volatile Vector *dst= (Vector*)activeDevice->lockVertexBuffer( vb->getVertexBuffer() );
   Vector *src= geo->getVertices();
   for (int i=0;i<geo->getVertexCount();i++)
   {
      dst[i].x= src[i].x;
      dst[i].y= src[i].y;
      dst[i].z= src[i].z;
   }
   activeDevice->unlockVertexBuffer( vb->getVertexBuffer() );

   activeDevice->allocateIndexBuffer(vb->getIndexBuffer(), geo->getEdgeCount()*2*sizeof(unsigned short), true );

   mVB.add(Material::Buffer(geo,vb));

/*
// pimped-up renderbuffer for cartoon shading
// store normals, texture-coordinates and dynamic buffer of visible siluette edges
class OutlineBuffer : public RenderBuffer
{
public:
OutlineBuffer(Geometry *geo) : RenderBuffer(geo)
{
mVertexCount= geo->getVertexCount();

mVertex= createVertexBuffer(geo->getVertices(), mVertexCount*sizeof(Vector));
mIndex=    createIndexBuffer(NULL, geo->getEdgeCount()*2*sizeof(unsigned short), false);
}
};
*/


//   OutlineBuffer *vb= new OutlineBuffer(geo);
//   mVB.add(vb);
}


int Outlining::calcEdgeIndices(unsigned int indexbuffer, Geometry *geo, const Vector& vw)
{
   Vector n1,n2;
   float  dir1, dir2;
   int    count;
   int    num=0;

   unsigned short *idx= (unsigned short*)activeDevice->lockIndexBuffer(indexbuffer);
   Vector *verts= geo->getVertices();
   Edge *edge= geo->getEdges();
   count= geo->getEdgeCount();
   for (int j=0;j<count;j++)
   {
      // vertices of two triangles with shared edge v1->v2
      const Vector& v1= verts[edge->i1];
      const Vector& v2= verts[edge->i2];
      const Vector& v3= verts[edge->i3];
      const Vector& v4= verts[edge->i4];

      // takes 8.5MI for 90.000 edges
      dir1= ((v2.y - v1.y)*(v3.z - v1.z) - (v2.z - v1.z)*(v3.y - v1.y)) * (v1.x - vw.x) +
         ((v2.z - v1.z)*(v3.x - v1.x) - (v2.x - v1.x)*(v3.z - v1.z)) * (v1.y - vw.y) +
         ((v2.x - v1.x)*(v3.y - v1.y) - (v2.y - v1.y)*(v3.x - v1.x)) * (v1.z - vw.z);

      dir2= ((v2.y - v1.y)*(v1.z - v4.z) - (v2.z - v1.z)*(v1.y - v4.y)) * (v1.x - vw.x) +
         ((v2.z - v1.z)*(v1.x - v4.x) - (v2.x - v1.x)*(v1.z - v4.z)) * (v1.y - vw.y) +
         ((v2.x - v1.x)*(v1.y - v4.y) - (v2.y - v1.y)*(v1.x - v4.x)) * (v1.z - vw.z);

      // draw edge if:
      // - one poly facing to viewer, one face away
      // - both polys facing to viewer and normals discontinue (precalc'd in flags)
      if ( (dir1*dir2)<0 || (dir1<0 && dir2<0 && edge->flags))
      {
         *idx++= edge->i1;
         *idx++= edge->i2;
         num+=2;
      }

      edge++;
   }

   activeDevice->unlockIndexBuffer(indexbuffer);

   return num;
}


void Outlining::update(float, Node**, const Matrix&)
{
   for (int i=0; i<mVB.size(); i++)
   {
      Geometry *geo= mVB[i].geo;
      // inverse-transform camera position to local object space
      Matrix invView= (geo->getTransform() * mCamera).invert();
      Vector viewer= invView.translation();

      int count= calcEdgeIndices(mVB[i].vb->getIndexBuffer(), mVB[i].geo, viewer);
      mVB[i].vb->setIndexCount(count);
   }
}

void Outlining::begin()
{
   Material::begin();

   // set material parameters
   //   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);

   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data

   activeDevice->setShader(mShader); // just transform from object to camera

   // pass2: render outlines
   glColor4f(0.0, 0.0, 0.0, 1.0); // black
   glEnable(GL_BLEND);

   glDepthMask(false);
}


void Outlining::end()
{
   glDepthMask(true);

   // disable vertex array
   glDisableClientState(GL_VERTEX_ARRAY);

   glDisable(GL_BLEND);

   activeDevice->setShader(0);
}


void Outlining::renderDiffuse()
{
   begin();

   for (int i=0;i<mVB.size();i++)
   {
      // set transformation
      activeDevice->push(mVB[i].geo->getTransform());

      glBindBuffer( GL_ARRAY_BUFFER_ARB, mVB[i].vb->getVertexBuffer() );
      glVertexPointer( 3, GL_FLOAT, 0, NULL );

      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, mVB[i].vb->getIndexBuffer() ); // created in precalc()
      glDrawElements( GL_LINES, mVB[i].vb->getIndexCount(), GL_UNSIGNED_SHORT, NULL ); // render

      activeDevice->pop();
   }

   end();
}

