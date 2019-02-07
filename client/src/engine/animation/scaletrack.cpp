#include "stdafx.h"
#include "scaletrack.h"


/*
ptm = position component (t)
rtm = "essential" rotation (q)

srtm = "stretch" rotation (u)
stm = scale component (k)

ftm = the flip tm -> ScaleMatrix(Point3(ap.f,ap.f,ap.f));


Here's the correct way of reassembling the decomposed matrix:
Matrix3 srtm, rtm, ptm, stm, ftm;
ptm.IdentityMatrix();
ptm.SetTrans(ap.t);
ap.q.MakeMatrix(rtm);
ap.u.MakeMatrix(srtm);
stm = ScaleMatrix(ap.k);

mat = Inverse(srtm) * stm * srtm * rtm * ftm * ptm;
*/

ScaleTrack::ScaleTrack()
: Track<ScaleKey>( Track::idScale )
{
}

void ScaleTrack::add(int time, const Scale& scale)
{
   addKey( ScaleKey(time, scale) );
}

void ScaleTrack::load(Stream *stream)
{
   Track<ScaleKey>::load(stream);

   for (int i=1;i<mSize;i++)
   {
      const Quat& q1= mData[i-1].value().orientation();
      const Quat& q2= mData[i].value().orientation();
      Quat q= q1*q2;
      if ((q1 % q) < 0) q=-q;
      Scale scale( q, mData[i].value().value() );
      mData[i].setValue( scale );
   }
}

void ScaleTrack::write(Stream *stream)
{
   // TODO: delta!
/*
   for (int i=1;i<mSize;i++)
   {
      Quat q1= mData[i-1].orientation();
      Quat q2= mData[i].orientation();
      Quat q= q1*q2;
      if ((q1 % q) < 0) q=-q;
      mData[i].set( q );
   }
*/

   Track<ScaleKey>::write(stream);
}


Matrix ScaleTrack::get(float time)
{
   Matrix rtm, stm;

   int last= size()-1;

   if (last<0)
   {
      stm.identity();
      return stm;
   }

   if (time<=key(0).time())
   {
      rtm= Matrix(key(0).value().orientation());
      stm= Matrix(key(0).value().value());
   }
   else
   if (time>=key(last).time())
   {
      rtm= Matrix(key(last).value().orientation());
      stm= Matrix(key(last).value().value());
   }
   else
   {
      float f= interpolate(time);

      const Vector& v1= prevKey().value().value();
      const Vector& v2= nextKey().value().value();
      Vector v= v1 + (v2-v1)*f;

      const Quat& q1= prevKey().value().orientation();
      const Quat& q2= nextKey().value().orientation();
      Quat q= q1 + (q2-q1)*f;
//      q= prevKey().orientation();

      rtm= Matrix(q);
      stm= Matrix(v);
   }

   Matrix irm= rtm.invert();
   return irm * stm * rtm;
}

