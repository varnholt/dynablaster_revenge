#include "bone.h"
#include "../nodes/scenegraph.h"

Bone::Bone()
: mID(-1)
, mInitTM()
{
}

Bone::Bone(const Bone& bone)
: mID(bone.id())
, mInitTM(bone.transform())
, mWeights( bone.weightList() )
{
}

Bone::Bone(const Bone& bone, Array<int> *remap)
: mID(bone.id())
, mInitTM(bone.transform())
{
   const List<Weight>& weights= bone.weightList();
   for (int w=0;w<weights.size();w++)
   {
      const Weight& weight= weights[w];
      const Array<int>& list= remap[weight.id()];
      for (int i=0;i<list.size();i++)
         mWeights.add( Weight(list[i], weight.weight()) );
   }
}

Bone& Bone::operator = (const Bone& bone)
{
   if (this != &bone)
   {
      mID= bone.id();
      mInitTM= bone.transform();
      mWeights= bone.weightList();
   }
   return *this;
}

Bone::~Bone()
{
}

int Bone::id() const
{
   return mID;
}

void Bone::setId(int id)
{
   mID= id;
}

int Bone::count() const
{
   return mWeights.size();
}

const Matrix& Bone::transform() const
{
   return mInitTM;
}

Weight* Bone::weights() const
{
   return mWeights.data();
}

const List<Weight>& Bone::weightList() const
{
   return mWeights;
}

void Bone::load(Stream* stream)
{
   mID= stream->getInt();
   mInitTM.load(stream);
   mWeights.load(stream);

   int verts[10000];
   for (int i=0; i<10000; i++)
      verts[i]= 0;

   int removed= 0;
   float sum= 0.0f;
   for (int i=0; i<mWeights.size(); i++)
   {
      const Weight& weight= mWeights[i];
      float w= weight.weight();
      if (weight.id() < 10000)
         verts[weight.id()]++;
      sum += w;
      if (w < 0.05f)
      {
         removed++;
      }
   }

   for (int i=0; i<10000; i++)
   {
      if (verts[i]>3)
      {
         printf("vertex %d has %d weights \n", i, verts[i]);
         verts[i]= 0;
      }
   }

   if (removed > 0)
   {
//      printf("%d / %d weights below threshold \n", removed, mWeights.size());
   }

   SceneGraph *scene= SceneGraph::instance();
   if (scene) mID += scene->getNodeStartIndex();
}

void Bone::write(Stream* stream)
{
   stream->writeInt(mID);
   mInitTM.write(stream);
   
   mWeights.write(stream);
}

