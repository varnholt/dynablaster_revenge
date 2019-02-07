#include "stdafx.h"
#include "playermaterialbase.h"
#include "nodes/mesh.h"
#include "gldevice.h"
#include "render/renderbuffer.h"
#include "render/uv.h"
#include "render/vertexbuffer.h"
#include "render/texturepool.h"
#include "tools/stream.h"
#include "animation/motionmixer.h"

Array<PlayerMaterialBase::Cluster*> PlayerMaterialBase::mClusters;

Vector PlayerMaterialBase::getCenter2d(const Matrix& projMat) const
{
   Vector center(0.0f);

   Geometry *geo= getGeometry(0);
   if (geo)
   {
      Node* mesh= geo->getParent();
      Matrix mat= mesh->getTransform() * projMat;

      float t= 1.0f / mat.ww;
      center.x= mat.xw * t;
      center.y= mat.yw * t;
      center.z= 0.0f;
   }
   return center;
}


void PlayerMaterialBase::getBoundingRect(
   Vector& min,
   Vector& max,
   const Matrix& projMat
)
{
   // realistic world space coordinates are somehwere around 0..13
   min= Vector(10000.0f, 10000.0f, 0.0f);
   max= Vector(-10000.0f, -10000.0f, 0.0f);

   for (int j=0; j<geometryCount(); j++)
   {
      Geometry *geo= getGeometry(j);
      Node* mesh= geo->getParent();

      Matrix mat= mesh->getTransform() * projMat;

      // get bounding box
      Array<Vector> vtx= geo->getSkinVertices();
      const int nv= geo->getVertexCount();
      for (int i=0; i<nv; i++)
      {
         const Vector& v= vtx[i];
         float x= mat.xx*v.x +  mat.xy*v.y +  mat.xz*v.z + mat.xw;
         float y= mat.yx*v.x +  mat.yy*v.y +  mat.yz*v.z + mat.yw;
         float z= mat.zx*v.x +  mat.zy*v.y +  mat.zz*v.z + mat.zw;
         float w= mat.wx*v.x +  mat.wy*v.y +  mat.wz*v.z + mat.ww;

         float t= 1.0f / w;
         x *= t;
         y *= t;
         z = 0.0f;

         max.maximum( Vector(x,y,z) );
         min.minimum( Vector(x,y,z) );
      }
   }
}


void PlayerMaterialBase::Cluster::mergeBones(const Array<int>& list, Array<int>& usages)
{
   const int size= list.size();
   for (int i=0; i<size; i++)
   {
      int idx= list[i];
      usages[idx]--;
      if (mBones.indexOf(idx) < 0)
         mBones.add(idx);
   }
}

void PlayerMaterialBase::Cluster::addVertexIndex(int v)
{
   int idx= mVertices.indexOf(v);
   if (idx < 0) idx= mVertices.add(v);
   mIndices.add(idx);
}

int PlayerMaterialBase::Cluster::boneCount() const
{
   return mBones.size();
}

bool PlayerMaterialBase::Cluster::containsBone(int id) const
{
   return mBones.indexOf(id) >= 0;
}

PlayerMaterialBase::PlayerMaterialBase(SceneGraph *scene, int id)
: Material(scene, id)
{
}


Array<PlayerMaterialBase::Cluster*> PlayerMaterialBase::createSkinClusters(Geometry* geo, int limit)
{
   Array<Cluster*> clusters;

   FaceList faces;
   faces.copy(geo->getIndicesList());

   // original data set is
   // bone 1: [vtx1, w1], [vtx2, w2], ...
   // bone 2: [vtx1, w1], [vtx2, w2], ...

   // create reverse order:
   // vtx1: [bone1, w1], [bone2, w2] ...
   // vtx2: [bone1, w1], [bone2, w2] ...

   Array<int> *vertexBones= new Array<int>[ geo->getVertexCount() ];
   Array<float> *vertexWeights= new Array<float>[ geo->getVertexCount() ];
   Array<int> boneUsage(geo->getBoneCount());
   for (int b=0;b<geo->getBoneCount();b++)
   {
      const Bone& bone= geo->getBone(b);
      Weight* weights= bone.weights();
      boneUsage[b]= bone.count();
      for (int v=0;v<bone.count();v++)
      {
         int idx= weights[v].id();
         vertexBones[idx].add( bone.id() );
         vertexWeights[idx].add( weights[v].weight() );
      }
   }

/*
   int maxTriWeights= 0;
   int maxVtxWeights= 0;
   for (int i=0; i<geo->getIndexCount(); i+=3)
   {
      Array<int> usedBones;
      for (int tri=0; tri<3; tri++)
      {
         const Array<int>& vtxWeights= vertexBones[ mIndices[i+tri] ];
         if (vtxWeights.size() > maxVtxWeights)
            maxVtxWeights= vtxWeights.size();
         for (int i=0; i<vtxWeights.size(); i++)
         {
            int bone= vtxWeights[i];
            if (usedBones.indexOf(bone) < 0)
                usedBones.add(bone);
         }
      }
      if (usedBones.size() > maxTriWeights)
         maxTriWeights= usedBones.size();
   }

   printf("maximum number of weights on a single vertex:   %d \n", maxVtxWeights);
   printf("maximum number of weights on a single triangle: %d \n", maxTriWeights);
*/


   Cluster* cluster= 0;
   int tris= 0;
   while (faces.size() > 0)
   {
      if (!cluster)
      {
         cluster= new Cluster();
         tris=0;
      }

      // todo: find triangle with the best match of vertex weights
      int bestMatch= 0; // maximize number of matching bones
      int bestNew= 100; // minimize number of new bones
      int bestTri= -1;
      int bestUse= 10000;
      for (int i=0; i<faces.size(); i+=3)
      {
         int curMatch= 0;
         int curNew= 0;
         int curUse= 1000;
         for (int tri=0; tri<3; tri++)
         {
            const Array<int>& vtxWeights= vertexBones[ faces[i+tri] ];
            for (int i=0; i<vtxWeights.size(); i++)
            {
               int bone= vtxWeights[i];
               if (cluster->containsBone(bone))
               {
                  curMatch++;
               }
               else
               {
                  curNew++;
                  if (curUse > boneUsage[bone])
                     curUse= boneUsage[bone];
               }
            }
         }

         if (curNew <= bestNew)
         {
            if (curNew < bestNew || curMatch >= bestMatch || curUse < bestUse)
            {
               bestTri= i;
               bestNew= curNew;
               bestMatch= curMatch;
               bestUse= curUse;
            }
         }
      }

      if (cluster->boneCount()+bestNew <= limit)
      {
         // add triangle to cluster
         for (int tri=0; tri<3; tri++)
         {
            int idx= faces[bestTri];
            cluster->mergeBones(vertexBones[idx], boneUsage);

            cluster->addVertexIndex(idx);
            faces.erase(bestTri); // index "bestTri+1" becomes "bestTri"
         }
         tris++;
      }
      else
      {
         // no triangle was found which fits into the given maximum of bone influences
//         printf("cluster %d:  %d tris   %d bones \n ", clusters.size(), tris, cluster->boneCount() );
         clusters.add( cluster );
         cluster= 0;
         // create new cluster in next iteration
      }
   }

   if (cluster)
      clusters.add(cluster);

   // figure weights for remapped vertices
   for (int c=0; c<clusters.size(); c++)
   {
      Cluster* cluster= clusters[c];

//      printf("cluster %d: \n", c);
      for (int i=0; i<cluster->mVertices.size(); i++)
      {
         int vtx= cluster->mVertices[i];
         const Array<int>& vtxBones= vertexBones[vtx];
         const Array<float>& vtxWeights= vertexWeights[vtx];

         float* weights= new float[limit];

         // build shared bone/weight list
         for (int b=0; b<cluster->mBones.size(); b++)
         {
            int idx= vtxBones.indexOf(cluster->mBones[b]);
            if (idx>=0)
               weights[b]= vtxWeights[idx];
            else
               weights[b]= 0.0f;
         }

         for (int b=cluster->mBones.size(); b<limit; b++)
            weights[b]= 0.0f;

         cluster->mWeights.add( weights );
      }
   }

   delete[] vertexBones;
   delete[] vertexWeights;

   return clusters;
}


void PlayerMaterialBase::addGeometry(Geometry *geo)
{
   VertexBuffer *vb= mPool->get(geo);
   if (!vb)
   {
      vb= mPool->add(geo);

      if (mClusters.size() == 0)
         mClusters= createSkinClusters(geo, 8);

      // get total number of vertices from clusters
      int totalVertices= 0;
      int totalIndices= 0;
      for (int i=0; i<mClusters.size(); i++)
      {
         totalVertices += mClusters[i]->mVertices.size();
         totalIndices += mClusters[i]->mIndices.size();
      }

      activeDevice->allocateVertexBuffer(vb->getVertexBuffer(), sizeof(Vertex)*totalVertices);
      {
         volatile Vertex *dst= (Vertex*)activeDevice->lockVertexBuffer(vb->getVertexBuffer());
         for (int i=0; i<mClusters.size(); i++)
         {
            Cluster* cluster= mClusters[i];

            Vector *vtx= geo->getVertices();
            Vector *nrm= geo->getNormals();
            UV *uv= geo->getUV(1);

            for (int i=0;i<cluster->mVertices.size();i++)
            {
               int idx= cluster->mVertices[i];
               dst->pos.x= vtx[idx].x;
               dst->pos.y= vtx[idx].y;
               dst->pos.z= vtx[idx].z;

               dst->normal.x= nrm[idx].x;
               dst->normal.y= nrm[idx].y;
               dst->normal.z= nrm[idx].z;

               dst->uv.u= uv[idx].u;
               dst->uv.v= uv[idx].v;

               for (int j=0; j<8; j++) dst->weight[j]= cluster->mWeights[i][j];
               dst++;
            }
         }
         activeDevice->unlockVertexBuffer(vb->getVertexBuffer());
      }

      {
         activeDevice->allocateIndexBuffer( vb->getIndexBuffer(), totalIndices*sizeof(unsigned short) );
         volatile unsigned short *dst= (unsigned short*)activeDevice->lockIndexBuffer( vb->getIndexBuffer() );
         int offset= 0;
         for (int i=0; i<mClusters.size(); i++)
         {
            Cluster* cluster= mClusters[i];
            for (int i=0; i<cluster->mIndices.size(); i++)
               *dst++= cluster->mIndices[i] + offset;
            offset += cluster->mVertices.size();
         }
         activeDevice->unlockIndexBuffer( vb->getIndexBuffer() );
      }

      vb->setIndexCount( totalIndices );

   }
   mVB.add( Material::Buffer(geo, vb) );
}


void PlayerMaterialBase::update(float /*frame*/, Node** /* nodelist */, const Matrix& cam )
{
   mCamera= cam;
}

