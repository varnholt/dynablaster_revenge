#include "stdafx.h"
#include "morphtrack.h"

MorphTrack::MorphTrack()
: Track<MorphKey>( Track::idVertexMorph )
{
}

void MorphTrack::add(int time, const List<Vector>& list)
{
   addKey( MorphKey(time, list) );
}

void MorphTrack::load(Stream *stream)
{
   Track<MorphKey>::load(stream);
   if (size()>0)
   {
      const MorphKey& key= Track<MorphKey>::get(0);
      mVertices.init( key.value().size() );
   }
}

const List<Vector>& MorphTrack::get(float time)
{
   int last= size()-1;

   if (last<0) return mVertices;

   if (time<=key(0).time()) return key(0).value();
   if (time>=key(last).time()) return key(last).value();

   float f= interpolate(time);

   const List<Vector>& v1= prevKey().value();
   const List<Vector>& v2= nextKey().value();

   for (int i=0; i<mVertices.size(); i++)
   {
      mVertices[i]= v1[i] + (v2[i]-v1[i])*f;
   }

   return mVertices;
}

