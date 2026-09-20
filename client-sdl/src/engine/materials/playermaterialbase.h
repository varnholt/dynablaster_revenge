#pragma once

#include "material.h"
#include "math/matrix.h"
#include "render/uv.h"

class PlayerMaterialBase : public Material
{
public:
   class Cluster
   {
   public:
      void mergeBones(const Array<int>& list, Array<int>& usages);
      void addVertexIndex(int vertexIndex);
      int boneCount() const;
      bool containsBone(int id) const;

      Array<int> mBones;
      Array<int> mVertices;
      Array<unsigned short> mIndices;
      Array<float*> mWeights;
   };

   struct Vertex {
      Vector pos;
      Vector normal;
      UV     uv;
      float  weight[8];
   };

   PlayerMaterialBase(SceneGraph *scene, int id);

   void update(float frame, Node **nodelist, const Matrix& cam);
   void addGeometry(Geometry *geo);

   virtual void getBoundingRect(Vector& min, Vector& max, const Matrix& projMat);
   virtual Vector getCenter2d(const Matrix& projMat) const;

private:
   Array<Cluster*> createSkinClusters(Geometry* geo, int limit);

protected:
   static Array< Cluster* > mClusters;
   Matrix mCamera;
};

