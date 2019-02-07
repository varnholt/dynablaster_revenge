#include "stdafx.h"
#include "mesh.h"
#include "tools/stream.h"
#include "math/vector.h"
#include "renderdevice.h"
#include "animation/motionmixer.h"
#include "tools/profiling.h"

Mesh::Mesh(Node *parent)
: Node(Node::idMesh, parent)
, mSkeleton(0)
, mMotionMixer(0)
, mAnimFrame(0.0f)
, mRenderFlags(0)
{
   for (int i=0;i<4; i++) mRenderParameter[i]= 0.0f;
}

Mesh::Mesh(const Mesh& mesh, Node* parent)
: Node(mesh, parent)
, mSkeleton( mesh.getSkeleton() )
, mMotionMixer( mesh.getMotionMixer() )
, mAnimFrame(0.0f)
{
   for (int i=0;i<mesh.getPartCount();i++)
   {
	   Geometry *p= mesh.getPart(i);
	   Geometry *geo= new Geometry( *p );
	   geo->setParent(this);
      add(geo);
   }

   setUserTransformable(true);
}

Mesh::~Mesh()
{
}

void Mesh::copy(const Mesh& mesh)
{
   if (!mParent)
      mParent= mesh.parent();
   mHasSkinning= mesh.hasSkinning();
   mUserTransform= mesh.getUserTransformable();
   mFrame= mesh.getFrame();
   mAnimFrame= mesh.getAnimationFrame();
   mSkeleton= mesh.getSkeleton();
   mMotionMixer= mesh.getMotionMixer();

   mBake= mesh.getBakedAnimation();

   for (int i=0;i<mesh.getPartCount();i++)
   {
      Geometry *geo= new Geometry(this);
      geo->copy( *(mesh.getPart(i)) );
      add(geo);
   }
}

void Mesh::setAnimationFrame(float frame)
{
   mAnimFrame= frame;
}

float Mesh::getAnimationFrame() const
{
   return mAnimFrame;
}

void Mesh::add(Geometry *geo)
{
   mGeometry.add(geo);
}

int Mesh::getPartCount() const
{
   return mGeometry.size();
}


Geometry* Mesh::getPart(int index) const
{
   return mGeometry[index];
}

Node* Mesh::getSkeleton() const
{
   return mSkeleton;
}

void Mesh::setSkeleton(Node *node)
{
   mSkeleton= node;
}

MotionMixer* Mesh::getMotionMixer() const
{
   return mMotionMixer;
}

void Mesh::setMotionMixer(MotionMixer* mixer)
{
   mMotionMixer= mixer;
}

void Mesh::transform(float frame)
{
   Node::transform(frame);
   MotionMixer *mixer= getMotionMixer();
   if (!mixer)
      return;

   double t1= getCpuTick();

   mixer->animate( getFrame() );

   double t2= getCpuTick();
/*
   for (int i=0; i<mGeometry.size(); i++)
   {
      Geometry* geo= mGeometry[i];

      int nv= geo->getVertexCount();
      Vector *vtemp= geo->getSkinVertices();
      Vector *ntemp= geo->getSkinNormals();
      memset(vtemp, 0, sizeof(Vector)*nv);
      memset(ntemp, 0, sizeof(Vector)*nv);

      Vector *vtx= geo->getVertices();
      Vector *nrm= geo->getNormals();

      for (int b=0;b<geo->getBoneCount();b++)
      {
         const Bone& bone= geo->getBone(b);

         Node *node= mixer->getNode( bone.id() );

//         Matrix bm= bone.transform() * node->getTransform();
         const Matrix& bm= node->getTransform();
         Matrix nbm= bm.normalize();

         Weight* weights= bone.weights();
         for (int v=0;v<bone.count();v++)
         {
            int idx= weights[v].id();
            float f= weights[v].weight();

            vtemp[idx] += (bm * vtx[idx]) * f;
            ntemp[idx] += (nbm * nrm[idx]) * f;
         }
      }
   }
*/

   double t3= getCpuTick();

   t1= (t2-t1)/1000000.0;
   t2= (t3-t2)/1000000.0;

//   printf("skin performance: %f / %f ms \r", t1, t2);
}


unsigned int Mesh::getRenderFlags() const
{
   return mRenderFlags;
}


void Mesh::setRenderFlags(unsigned int flags)
{
   mRenderFlags= flags;
}


float Mesh::getRenderParameter(int index) const
{
   return mRenderParameter[index];
}


void Mesh::setRenderParameter(int index, float param)
{
   mRenderParameter[index]= param;
}


void Mesh::load(Stream *stream)
{
   Node::load(stream);

   /*int flags=*/ stream->getInt();

   // int castshadow= flags & 1;
   // int recvshadow= flags & 2;

   int ng= stream->getInt();
   mGeometry.init(ng);
   for (int i=0;i<ng;i++)
   {
	   Geometry *geo= new Geometry(this);
	   geo->load(stream);
	   mGeometry.add(geo);
   }

/*
   for (int i=0;i<ng;i++) 
	   if (mGeometry[i]->getWeightCount()>0) 
		   mHasSkinning= true;
*/

   // morph targets
//   int mt= stream->getInt();

   /*
   for (int i=0;i<4;i++)
   {
      for (int j=0;j<3;j++)
      {
         float p= stream->getFloat();
         printf("%f ", p);
      }
      printf("\n");
   }
*/
   // read tracks
   Chunk anim(stream);
   mPosTrack.load(&anim);
   mRotTrack.load(&anim);
   mScaleTrack.load(&anim);
   mVisTrack.load(&anim);
//   mFlipTrack.load(&anim);
   anim.skip();

}

void Mesh::write(Stream *stream)
{
   Node::write(stream);

   int flags= 0; // TODO!
   stream->writeInt(flags);

   stream->writeInt( mGeometry.size() );
   for (int i=0; i<mGeometry.size(); i++)
   {
	   Geometry *geo= mGeometry[i];
	   geo->write(stream);
   }


   // write tracks
   Chunk anim(stream, 2000, "Animation");
   mPosTrack.write(&anim);
   mRotTrack.write(&anim);
   mScaleTrack.write(&anim);
   mVisTrack.write(&anim);
   mFlipTrack.write(&anim);
}


// create box mapping
void Mesh::createBoxMapping(bool unwrap, const Vector& min, const Vector& max, const Matrix& tm)
{
   for (int i=0; i<mGeometry.size(); i++)
   {
      mGeometry[i]->createBoxMapping(unwrap, min, max, getTransform(), tm);
   }
}
