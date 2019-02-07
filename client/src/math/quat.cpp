#include "stdafx.h"
#include "quat.h"
#include "tools/stream.h"
#include "matrix.h"
#include "vector.h"
#include <math.h>

Quat::Quat()
{
}


Quat::Quat(float nx, float ny, float nz, float nw)
{
   x= nx;
   y= ny;
   z= nz;
   w= nw;
}

/* This algorithm avoids near-zero divides by looking for a large component
   * - first w, then x, y, or z.  When the trace is greater than zero,
   * |w| is greater than 1/2, which is as small as a largest component can be.
   * Otherwise, the largest diagonal entry corresponds to the largest of |x|,
   * |y|, or |z|, one of which must be larger than |w|, and at least 1/2. */
Quat::Quat(const Matrix& mat)
{
	float tr, s;

	tr = mat.xx + mat.yy + mat.zz;
	if (tr >= 0.0) 
   {
      // W
		s = sqrt(tr + mat.ww);
		w = (float)s*0.5f;
		s = 0.5f / s;
		x = (mat.zy - mat.yz) * s;
		y = (mat.xz - mat.zx) * s;
		z = (mat.yx - mat.xy) * s;
	} 
   else 
   {
      if (mat.xx > mat.yy && mat.xx > mat.zz)
      {
         // X
         s = sqrt( mat.xx - (mat.yy+mat.zz) + mat.ww );
         x = s*0.5f;
         s = 0.5f / s;
         y = (mat.xy + mat.yx) * s;
         z = (mat.zx + mat.xz) * s;
         w = (mat.zy - mat.yz) * s;
      }
      else
      if (mat.yy > mat.zz)
      {
         // Y
         s = sqrt( mat.yy - (mat.zz+mat.xx) + mat.ww );
         y = s*0.5f;
         s = 0.5f / s;
         z = (mat.yz + mat.zy) * s;
         x = (mat.xy + mat.yx) * s;
         w = (mat.xz - mat.zx) * s;
      }
      else
      {
         // Z
         s = sqrt( mat.zz - (mat.xx+mat.yy) + mat.ww );
         z = s*0.5f;
         s = 0.5f / s;
         x = (mat.zx + mat.xz) * s;
         y = (mat.yz + mat.zy) * s;
         w = (mat.yx - mat.xy) * s;
      }
	}

	if (mat.ww != 1.0) 
   {
      float t= 1.0f / sqrt( mat.ww );
      (*this) = (*this) * t;
   }
}

// negation
Quat Quat::operator-() const
{
   Quat n;
   n.x= -x;
   n.y= -y;
   n.z= -z;
   n.w= -w;
   return n;
}


// add
Quat Quat::operator + (const Quat& q) const
{
   Quat n;

   n.x= x + q.x;
   n.y= y + q.y;
   n.z= z + q.z;
   n.w= w + q.w;

   return n;
}


// sub
Quat Quat::operator - (const Quat& q) const
{
   Quat n;

   n.x= x - q.x;
   n.y= y - q.y;
   n.z= z - q.z;
   n.w= w - q.w;

   return n;
}


// scalar multiply
Quat Quat::operator * (const float f) const
{
   Quat n;

   n.x= x * f;
   n.y= y * f;
   n.z= z * f;
   n.w= w * f;

   return n;
}


// quaternion multiply
Quat Quat::operator * (const Quat& q) const
{
   Quat n;

   n.x = w*q.x + x*q.w + y*q.z - z*q.y;
   n.y = w*q.y + y*q.w + z*q.x - x*q.z;
   n.z = w*q.z + z*q.w + x*q.y - y*q.x;
   n.w = w*q.w - x*q.x - y*q.y - z*q.z;

   return n;
}


// dot product
float Quat::operator % (const Quat& q) const
{
   return x*q.x + y*q.y + z*q.z + w*q.w;
}

void Quat::operator << (Stream& stream)
{
   load(&stream);
}

void Quat::operator >> (Stream& stream)
{
   write(&stream);
}

Quat Quat::conjugate() const
{
   return Quat( -x, -y, -z, w);
}

// load object from stream
void Quat::load(Stream *stream)
{
   x= stream->getFloat();
   y= stream->getFloat();
   z= stream->getFloat();
   w= stream->getFloat();
}

// load object from stream
void Quat::write(Stream *stream)
{
   stream->writeFloat(x);
   stream->writeFloat(y);
   stream->writeFloat(z);
   stream->writeFloat(w);
}

// spherical linear interpolation
Quat Quat::slerp(const Quat& q1, const Quat& q2, float t)
{
   float s1,s2;

   float v= q1 % q2;

   // avoid div 0
   if (v<0.9999999f)
   {
	  float a = acos( v );
      float s3= 1.0f / sin(a);
      s1= sin((1.0f-t) * a) * s3;
      s2= sin(t * a) * s3;
   }
   else
   {
      s1= 1.0f - t;
      s2= t;
   }

   Quat n= (q1 * s1) + (q2 * s2);
   return n;
}

/* 
Given a unit quaternion, q, and a scale vector, k, find a unit quaternion, p,
which permutes the axes and turns freely in the plane of duplicate scale
factors, such that q p has the largest possible w component, i.e. the
smallest possible angle. Permutes k's components to go with q p instead of q.
*/
Quat Quat::snuggle(Vector& k)
{
   const float sqrtHalf= 0.7071067811865475244f;
#define sgn(n,v)    ((n)?-(v):(v))
#define swap(a,i,j) {float t=a[i]; a[i]=a[j]; a[j]=t;}
#define cycle(a,p)  if (p) {float t=a[0]; a[0]=a[1]; a[1]=a[2]; a[2]=t;}\
             else   {float t=a[2]; a[2]=a[1]; a[1]=a[0]; a[0]=t;}

   Quat p;
   Quat q= *this;
   float ka[3];
   int i, turn = -1;
   ka[0] = k.x; 
   ka[1] = k.y; 
   ka[2] = k.z;
   if (ka[0]==ka[1])
   {
      if (ka[0]==ka[2]) 
         turn = 3; 
      else 
         turn = 2;
   }
   else 
   {
      if (ka[0]==ka[2]) 
         turn = 1; 
      else if (ka[1]==ka[2]) 
         turn = 0;
   }

   if (turn>=0) 
   {
      Quat qtoz, qp;
      unsigned neg[3], win;
      double mag[3], t;
      static Quat qxtoz( 0,sqrtHalf,0,sqrtHalf );
      static Quat qytoz( sqrtHalf,0,0,sqrtHalf );
      static Quat qppmm( 0.5f, 0.5f,-0.5f,-0.5f );
      static Quat qpppp( 0.5f, 0.5f, 0.5f, 0.5f );
      static Quat qmpmm(-0.5f, 0.5f,-0.5f,-0.5f );
      static Quat qpppm( 0.5f, 0.5f, 0.5f,-0.5f );
      static Quat q0001( 0.0f, 0.0f, 0.0f, 1.0f );
      static Quat q1000( 1.0f, 0.0f, 0.0f, 0.0f );

      switch (turn)
      {
         case 0: 
         {
            q = q * (qtoz = qxtoz); 
            float t= ka[0];
            ka[0] = ka[2];
            ka[2] = t;
            break;
         }

         case 1: 
         {
            q = q * (qtoz = qytoz); 
            float t= ka[1];
            ka[1] = ka[2];
            ka[2] = t;
            break;
         }

         case 2: 
            qtoz = q0001; 
            break;
         default: 
            return q.conjugate();
      }
      q = q.conjugate();
      mag[0] = (double)q.z*q.z+(double)q.w*q.w-0.5;
      mag[1] = (double)q.x*q.z-(double)q.y*q.w;
      mag[2] = (double)q.y*q.z+(double)q.x*q.w;
      
      for (i=0; i<3; i++)
      {
         neg[i]= (mag[i]<0.0);
         if (neg[i]) mag[i]= -mag[i];
      }

      if (mag[0]>mag[1]) 
      {
         if (mag[0]>mag[2]) win = 0; 
         else win = 2;
      }
      else	
      {
         if (mag[1]>mag[2]) win = 1; 
         else win = 2;
      }
      
      switch (win) 
      {
      case 0: 
         if (neg[0]) p = q1000; 
         else p = q0001; 
         break;
      case 1: 
         if (neg[1]) p = qppmm; 
         else p = qpppp; 
         cycle(ka,0) 
         break;
      case 2: 
         if (neg[2]) p = qmpmm; 
         else p = qpppm; 
         cycle(ka,1) 
         break;
      }
      qp = q * p;
      t = sqrt(mag[win]+0.5f);
      p = p * Quat(0.0f, 0.0f, (float)(-qp.z/t), (float)(qp.w/t));
      p = qtoz * p.conjugate();
   } 
   else 
   {
      float qa[4], pa[4];
      unsigned lo, hi, neg[4], par = 0;
      double all, big, two;
      qa[0] = q.x; 
      qa[1] = q.y; 
      qa[2] = q.z; 
      qa[3] = q.w;
      for (i=0; i<4; i++) 
      {
         pa[i] = 0.0;
         neg[i] = (qa[i]<0.0);
         if (neg[i]) qa[i] = -qa[i];
         par ^= neg[i];
      }

      /* Find two largest components, indices in hi and lo */
      if (qa[0]>qa[1]) lo = 0; 
      else lo = 1;
      if (qa[2]>qa[3]) hi = 2; 
      else hi = 3;
      if (qa[lo]>qa[hi]) 
      {
         if (qa[lo^1]>qa[hi]) 
         {
            hi = lo; 
            lo ^= 1;
         }
         else 
         {
            hi ^= lo; 
            lo ^= hi; 
            hi ^= lo;
         }
      } 
      else 
      {
         if (qa[hi^1]>qa[lo]) lo = hi^1;
      }

      all = (qa[0]+qa[1]+qa[2]+qa[3])*0.5;
      two = (qa[hi]+qa[lo])*sqrtHalf;
      big = qa[hi];
      if (all>two) 
      {
         if (all>big) 
         {
            {
               int i; 
               for (i=0; i<4; i++) pa[i] = sgn(neg[i], 0.5f);
            }
            cycle(ka,par)
         } 
         else 
         {/*big*/
            pa[hi] = sgn(neg[hi],1.0f);
         }
      } 
      else 
      {
         if (two>big) 
         {/*two*/
            pa[hi] = sgn(neg[hi],sqrtHalf); 
            pa[lo] = sgn(neg[lo], sqrtHalf);
            if (lo>hi) 
            {
               hi ^= lo; 
               lo ^= hi; 
               hi ^= lo;
            }
            if (hi==3) 
            {
               hi = "\001\002\000"[lo]; 
               lo = 3-hi-lo;
            }
            swap(ka,hi,lo)
         } 
         else 
         {
            // big
            pa[hi] = sgn(neg[hi],1.0f);
         }
      }
      p.x = -pa[0]; 
      p.y = -pa[1]; 
      p.z = -pa[2]; 
      p.w = pa[3];
   }

   k.x = ka[0]; 
   k.y = ka[1]; 
   k.z = ka[2];
   return p;
}
