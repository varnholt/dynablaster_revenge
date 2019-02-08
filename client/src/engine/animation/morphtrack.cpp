#include "stdafx.h"
#include "morphtrack.h"

MorphTrack::MorphTrack()
: Track<MorphKey>( Track::idVertexMorph )
{
}

void MorphTrack::load(Stream *stream)
{
   Track<MorphKey>::load(stream);
   if (size()>0)
   {
//      const MorphKey& key= Track<MorphKey>::get(0);
   }
}

void MorphTrack::get(List<Vector>& vtx, List<Vector>& nrm, float time)
{
   int i;

   const MorphKey& first= Track<MorphKey>::get(0);
   const MorphKey& last=  Track<MorphKey>::getLast();

   const int count= first.getVertexCount();

   if (time <= first.time())
   {
      const List<Vector>& verts= first.getVertices();
      const List<Vector>& normals= first.getNormals();
      for (i=0; i<count; i++)
      {
         vtx[i]= verts[i];
         nrm[i]= normals[i];
      }
   }
   else
   if (time >= last.time())
   {
      const List<Vector>& verts= last.getVertices();
      const List<Vector>& normals= last.getNormals();
      for (i=0; i<count; i++)
      {
         vtx[i]= verts[i];
         nrm[i]= normals[i];
      }
   }
   else
   {
      float f= interpolate(time);

      const List<Vector>& v1= prevKey().getVertices();
      const List<Vector>& n1= prevKey().getNormals();

      const List<Vector>& v2= nextKey().getVertices();
      const List<Vector>& n2= nextKey().getNormals();

      for (int i=0; i<count; i++)
      {
         vtx[i]= v1[i] + (v2[i]-v1[i])*f;
         nrm[i]= n1[i] + (n2[i]-n1[i])*f;
      }
   }
}

void MorphTrack::calculateNormals(const Array<unsigned short>& indices)
{
   int i;
   const int numKeys= size();
   for (i=0; i<numKeys; i++)
   {
      mData[i].calculateNormals(indices);
   }
}
