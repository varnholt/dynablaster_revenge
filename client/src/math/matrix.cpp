#include "stdafx.h"
#include "matrix.h"
#include "tools/stream.h"
#include "scale.h"
#include <math.h>
#include <stdio.h>

Matrix::Matrix()
: xx(1.0f), yx(0.0f), zx(0.0f), wx(0.0f)
, xy(0.0f), yy(1.0f), zy(0.0f), wy(0.0f)
, xz(0.0f), yz(0.0f), zz(1.0f), wz(0.0f)
, xw(0.0f), yw(0.0f), zw(0.0f), ww(1.0f)
{
}

Matrix::Matrix( float xx, float yx, float zx, float wx,
                float xy, float yy, float zy, float wy,
                float xz, float yz, float zz, float wz,
                float xw, float yw, float zw, float ww )
: xx(xx), yx(yx), zx(zx), wx(wx)
, xy(xy), yy(yy), zy(zy), wy(wy)
, xz(xz), yz(yz), zz(zz), wz(wz)
, xw(xw), yw(yw), zw(zw), ww(ww)
{
}

Matrix::Matrix(const Vector& v)
{
   xx= v.x;  xy= 0.0f; xz= 0.0f; xw= 0.0f;
   yx= 0.0f; yy= v.y;  yz= 0.0f; yw= 0.0f;
   zx= 0.0f; zy= 0.0f; zz= v.z;  zw= 0.0f;
   wx= 0.0f; wy= 0.0f; wz= 0.0f; ww= 1.0f;
}

Matrix::Matrix(const Vector& x, const Vector& y, const Vector& z)
{
   xx= x.x;  xy= y.x;  xz= z.x;  xw= 0.0f;
   yx= x.y;  yy= y.y;  yz= z.y;  yw= 0.0f;
   zx= x.z;  zy= y.z;  zz= z.z;  zw= 0.0f;
   wx= 0.0f; wy= 0.0f; wz= 0.0f; ww= 1.0f;
}

Matrix::Matrix(const Quat& q)
{
   float r2xx =  q.x * q.x * 2.0f;
   float r2yy =  q.y * q.y * 2.0f;
   float r2zz =  q.z * q.z * 2.0f;
   float r2xy =  q.x * q.y * 2.0f;
   float r2xz =  q.x * q.z * 2.0f;
   float r2yz =  q.y * q.z * 2.0f;
   float r2wx =  q.w * q.x * 2.0f;
   float r2wy =  q.w * q.y * 2.0f;
   float r2wz =  q.w * q.z * 2.0f;

   xx = 1.0f - r2yy - r2zz;
   xy = r2xy + r2wz;
   xz = r2xz - r2wy;
   xw = 0.0f;

   yx = r2xy - r2wz;
   yy = 1.0f - r2xx - r2zz;
   yz = r2yz + r2wx;
   yw = 0.0f;

   zx = r2xz + r2wy;
   zy = r2yz - r2wx;
   zz = 1.0f - r2xx - r2yy;
   zw = 0.0f;

   wx = 0.0f;
   wy = 0.0f;
   wz = 0.0f;
   ww = 1.0f;
}

void Matrix::load(Stream *stream)
{
   xx= stream->getFloat();
   yx= stream->getFloat();
   zx= stream->getFloat();
   wx= stream->getFloat();
   xy= stream->getFloat();
   yy= stream->getFloat();
   zy= stream->getFloat();
   wy= stream->getFloat();
   xz= stream->getFloat();
   yz= stream->getFloat();
   zz= stream->getFloat();
   wz= stream->getFloat();
   xw= stream->getFloat();
   yw= stream->getFloat();
   zw= stream->getFloat();
   ww= stream->getFloat();
}

void Matrix::write(Stream *stream)
{
   stream->writeFloat(xx);
   stream->writeFloat(yx);
   stream->writeFloat(zx);
   stream->writeFloat(wx);
   stream->writeFloat(xy);
   stream->writeFloat(yy);
   stream->writeFloat(zy);
   stream->writeFloat(wy);
   stream->writeFloat(xz);
   stream->writeFloat(yz);
   stream->writeFloat(zz);
   stream->writeFloat(wz);
   stream->writeFloat(xw);
   stream->writeFloat(yw);
   stream->writeFloat(zw);
   stream->writeFloat(ww);
}

Matrix Matrix::frustum(float l, float r, float b, float t, float n, float f)
{
   Matrix m;
   float invRL= 1.0f / (r-l);
   float invTB= 1.0f / (t-b);
   float invFN= 1.0f / (f-n);
   m.xx= 2.0f*n*invRL; m.xy= 0.0f;       m.xz= (r+l)*invRL; m.xw= 0.0f;
   m.yx= 0.0f;         m.yy= 2.0f*invTB; m.yz= (t+b)*invTB; m.yw= 0.0f;
   m.zx= 0.0f;         m.zy= 0.0f;       m.zz=-(f+n)*invFN; m.zw=-2.0f*f*n*invFN;
   m.wx= 0.0f;         m.wy= 0.0f;       m.wz= -1.0f;       m.ww= 0.0f;
   return m;
}

Matrix Matrix::ortho(float l, float r, float b, float t, float n, float f)
{
   Matrix m;
   float invRL= 1.0f / (r-l);
   float invTB= 1.0f / (t-b);
   float invFN= 1.0f / (f-n);
   m.xx= 2.0f*invRL; m.xy= 0.0f;       m.xz= 0.0f;        m.xw=-(r+l)*invRL;
   m.yx= 0.0f;       m.yy= 2.0f*invTB; m.yz= 0.0f;        m.yw=-(t+b)*invTB;
   m.zx= 0.0f;       m.zy= 0.0f;       m.zz=-2.0f*invFN;  m.zw=-(f+n)*invFN;
   m.wx= 0.0f;       m.wy= 0.0f;       m.wz= 0.0f;        m.ww= 1.0f;
   return m;
}

//! creates look-at matrix compatible to gluLookAt
//! up vector must be normalized
Matrix Matrix::lookAt(const Vector& eye, const Vector& center, const Vector& up)
{
   Vector z= (eye - center);
   z.normalize();

   Vector x= (-z % up);
   Vector y= (x % -z);

   Matrix m;
   m.xx= x.x;
   m.xy= x.y;
   m.xz= x.z;
   m.xw= -m.xx*eye.x - m.xy*eye.y - m.xz*eye.z;

   m.yx= y.x;
   m.yy= y.y;
   m.yz= y.z;
   m.yw= -m.yx*eye.x - m.yy*eye.y - m.yz*eye.z;

   m.zx= z.x;
   m.zy= z.y;
   m.zz= z.z;
   m.zw= -m.zx*eye.x - m.zy*eye.y - m.zz*eye.z;

   m.wx= 0.0f;
   m.wy= 0.0f;
   m.wz= 0.0f;
   m.ww= 1.0f;

   return m;
}

Matrix Matrix::scale(float sx, float sy, float sz)
{
   Matrix m;
   m.xx= sx;   m.xy= 0.0f; m.xz= 0.0f; m.xw= 0.0f;
   m.yx= 0.0f; m.yy= sy;   m.yz= 0.0f; m.yw= 0.0f;
   m.zx= 0.0f; m.zy= 0.0f; m.zz= sz;   m.zw= 0.0f;
   m.wx= 0.0f; m.wy= 0.0f; m.wz= 0.0f; m.ww= 1.0f;
   return m;
}

Matrix Matrix::position(const Vector& pos)
{
   Matrix m;
   m.xx= 1.0f; m.xy= 0.0f; m.xz= 0.0f; m.xw= pos.x;
   m.yx= 0.0f; m.yy= 1.0f; m.yz= 0.0f; m.yw= pos.y;
   m.zx= 0.0f; m.zy= 0.0f; m.zz= 1.0f; m.zw= pos.z;
   m.wx= 0.0f; m.wy= 0.0f; m.wz= 0.0f; m.ww= 1.0f;
   return m;
}

Matrix Matrix::position(float x, float y, float z)
{
   Matrix m;
   m.xx= 1.0f; m.xy= 0.0f; m.xz= 0.0f; m.xw= x;
   m.yx= 0.0f; m.yy= 1.0f; m.yz= 0.0f; m.yw= y;
   m.zx= 0.0f; m.zy= 0.0f; m.zz= 1.0f; m.zw= z;
   m.wx= 0.0f; m.wy= 0.0f; m.wz= 0.0f; m.ww= 1.0f;
   return m;
}

// vector= matrix * vector
Vector Matrix::operator * (const Vector& v) const
{
  Vector n;

   n.x= xx*v.x +  xy*v.y +  xz*v.z + xw;
   n.y= yx*v.x +  yy*v.y +  yz*v.z + yw;
   n.z= zx*v.x +  zy*v.y +  zz*v.z + zw;

  return n;
}

// vector= matrix * vector
Vector4 Matrix::operator * (const Vector4& v) const
{
   Vector4 n;

   n.x= xx*v.x +  xy*v.y +  xz*v.z + xw*v.w;
   n.y= yx*v.x +  yy*v.y +  yz*v.z + yw*v.w;
   n.z= zx*v.x +  zy*v.y +  zz*v.z + zw*v.w;
   n.w= wx*v.x +  wy*v.y +  wz*v.z + ww*v.w;

   return n;
}

// matrix * scalar
Matrix Matrix::operator * (const float s) const
{
   Matrix m;

   m.xx= xx*s; m.xy= xy*s; m.xz= xz*s; m.xw= xw*s;
   m.yx= yx*s; m.yy= yy*s; m.yz= yz*s; m.yw= yw*s;
   m.zx= zx*s; m.zy= zy*s; m.zz= zz*s; m.zw= zw*s;
   m.wx= wx*s; m.wy= wy*s; m.wz= wz*s; m.ww= ww*s;

   return m;
}

// matrix + matrix
Matrix Matrix::operator + (const Matrix& m) const
{
   Matrix n;

   n.xx= m.xx+xx; n.xy= m.xy+xy; n.xz= m.xz+xz; n.xw= m.xw+xw;
   n.yx= m.yx+yx; n.yy= m.yy+yy; n.yz= m.yz+yz; n.yw= m.yw+yw;
   n.zx= m.zx+zx; n.zy= m.zy+zy; n.zz= m.zz+zz; n.zw= m.zw+zw;
   n.wx= m.wx+wx; n.wy= m.wy+wy; n.wz= m.wz+wz; n.ww= m.ww+ww;

   return n;
}

// matrix - matrix
Matrix Matrix::operator - (const Matrix& m) const
{
   Matrix n;

   n.xx= xx-m.xx; n.xy= xy-m.xy; n.xz= xz-m.xz; n.xw= xw-m.xw;
   n.yx= yx-m.yx; n.yy= yy-m.yy; n.yz= yz-m.yz; n.yw= yw-m.yw;
   n.zx= zx-m.zx; n.zy= zy-m.zy; n.zz= zz-m.zz; n.zw= zw-m.zw;
   n.wx= wx-m.wx; n.wy= wy-m.wy; n.wz= wz-m.wz; n.ww= ww-m.ww;

   return n;
}

Matrix Matrix::operator * (const Matrix& m2) const
{
   Matrix n;

   n.xx= m2.xx*xx + m2.xy*yx + m2.xz*zx + m2.xw*wx;
   n.xy= m2.xx*xy + m2.xy*yy + m2.xz*zy + m2.xw*wy;
   n.xz= m2.xx*xz + m2.xy*yz + m2.xz*zz + m2.xw*wz;
   n.xw= m2.xx*xw + m2.xy*yw + m2.xz*zw + m2.xw*ww;

   n.yx= m2.yx*xx + m2.yy*yx + m2.yz*zx + m2.yw*wx;
   n.yy= m2.yx*xy + m2.yy*yy + m2.yz*zy + m2.yw*wy;
   n.yz= m2.yx*xz + m2.yy*yz + m2.yz*zz + m2.yw*wz;
   n.yw= m2.yx*xw + m2.yy*yw + m2.yz*zw + m2.yw*ww;

   n.zx= m2.zx*xx + m2.zy*yx + m2.zz*zx + m2.zw*wx;
   n.zy= m2.zx*xy + m2.zy*yy + m2.zz*zy + m2.zw*wy;
   n.zz= m2.zx*xz + m2.zy*yz + m2.zz*zz + m2.zw*wz;
   n.zw= m2.zx*xw + m2.zy*yw + m2.zz*zw + m2.zw*ww;

   n.wx= m2.wx*xx + m2.wy*yx + m2.wz*zx + m2.ww*wx;
   n.wy= m2.wx*xy + m2.wy*yy + m2.wz*zy + m2.ww*wy;
   n.wz= m2.wx*xz + m2.wy*yz + m2.wz*zz + m2.ww*wz;
   n.ww= m2.wx*xw + m2.wy*yw + m2.wz*zw + m2.ww*ww;

/*
  n.xx = m.xx*xx + m.xy*yx + m.xz*zx;
  n.xy = m.xx*xy + m.xy*yy + m.xz*zy;
  n.xz = m.xx*xz + m.xy*yz + m.xz*zz;
  n.xw = m.xx*xw + m.xy*yw + m.xz*zw + m.xw;

  n.yx = m.yx*xx + m.yy*yx + m.yz*zx;
  n.yy = m.yx*xy + m.yy*yy + m.yz*zy;
  n.yz = m.yx*xz + m.yy*yz + m.yz*zz;
  n.yw = m.yx*xw + m.yy*yw + m.yz*zw + m.yw;

  n.zx = m.zx*xx + m.zy*yx + m.zz*zx;
  n.zy = m.zx*xy + m.zy*yy + m.zz*zy;
  n.zz = m.zx*xz + m.zy*yz + m.zz*zz;
  n.zw = m.zx*xw + m.zy*yw + m.zz*zw + m.zw;

  n.wx = m.wx*xx + m.wy*yx + m.wz*zx;
  n.wy = m.wx*xy + m.wy*yy + m.wz*zy;
  n.wz = m.wx*xz + m.wy*yz + m.wz*zz;
  n.ww = m.wx*xw + m.wy*yw + m.wz*zw + m.ww;
*/

  return n;
}


void Matrix::identity()
{
  xx= 1.0f; xy= 0.0f; xz= 0.0f; xw= 0.0f;
  yx= 0.0f; yy= 1.0f; yz= 0.0f; yw= 0.0f;
  zx= 0.0f; zy= 0.0f; zz= 1.0f; zw= 0.0f;
  wx= 0.0f; wy= 0.0f; wz= 0.0f; ww= 1.0f;
}


Matrix Matrix::getView() const
{
   Vector v= -translation();
   Matrix n= transpose3x3();

   n.xw= xx*v.x +  yx*v.y +  zx*v.z;
   n.yw= xy*v.x +  yy*v.y +  zy*v.z;
   n.zw= xz*v.x +  yz*v.y +  zz*v.z;

   return n;
}


// return xyw columns as new matrix
Matrix Matrix::xyw() const
{
  Matrix n;

  n.xx= xx;   n.xy= xy;   n.xz= xz;   n.xw= xw;
  n.yx= yx;   n.yy= yy;   n.yz= yz;   n.yw= yw;
  n.zx= wx;   n.zy= wy;   n.zz= wz;   n.zw= ww;
  n.wx= 0.0f; n.wy= 0.0f; n.wz= 0.0f; n.ww= 1.0f;

  return n;
}


Matrix Matrix::normalized() const
{
   Matrix n;
   float t,m;

   t= 1.0f / sqrtf( xx*xx + xy*xy + xz*xz );
   n.xx = xx * t;
   n.xy = xy * t;
   n.xz = xz * t;
   n.xw = xw;

   t= 1.0f / sqrtf( yx*yx + yy*yy + yz*yz );
   n.yx = yx * t;
   n.yy = yy * t;
   n.yz = yz * t;
   n.yw = yw;

   t= 1.0f / sqrtf( zx*zx + zy*zy + zz*zz );
   n.zx = zx * t;
   n.zy = zy * t;
   n.zz = zz * t;
   n.zw = zw;

   m= wx*wx + wy*wy + wz*wz;
   if (m > 0.00000001f)
      t= 1.0f / sqrtf( m );
   else
      t= 0.0f;

   n.wx = wx * t;
   n.wy = wy * t;
   n.wz = wz * t;
   n.ww = ww;

   return n;

}


void Matrix::normalizeZ()
{
   float t= 1.0f / sqrt( zx*zx + zy*zy + zz*zz );
   zx *= t;
   zy *= t;
   zz *= t;

}

Matrix Matrix::get3x3() const
{
   Matrix n;
   n.xx = xx;
   n.xy = xy;
   n.xz = xz;
   n.xw = 0.0f;

   n.yx = yx;
   n.yy = yy;
   n.yz = yz;
   n.yw = 0.0f;

   n.zx = zx;
   n.zy = zy;
   n.zz = zz;
   n.zw = 0.0f;

   n.wx = 0.0f;
   n.wy = 0.0f;
   n.wz = 0.0f;
   n.ww = 1.0f;

   return n;
}


Matrix Matrix::transpose() const
{
   Matrix n;

   n.xx = xx;
   n.xy = yx;
   n.xz = zx;
   n.xw = wx;

   n.yx = xy;
   n.yy = yy;
   n.yz = zy;
   n.yw = wy;

   n.zx = xz;
   n.zy = yz;
   n.zz = zz;
   n.zw = wz;

   n.wx = xw; // pos.x
   n.wy = yw; // pos.y
   n.wz = zw; // pos.z
   n.ww = ww; // 1.0

   return n;
}


Matrix Matrix::transpose3x3() const
{
   Matrix n;

   n.xx = xx;
   n.xy = yx;
   n.xz = zx;
   n.xw = xw; // pos.x

   n.yx = xy;
   n.yy = yy;
   n.yz = zy;
   n.yw = yw; // pos.y

   n.zx = xz;
   n.zy = yz;
   n.zz = zz;
   n.zw = zw; // pos.z

   n.wx = wx;
   n.wy = wy;
   n.wz = wz;
   n.ww = ww; // 1.0

   return n;
}


void Matrix::translate(const Vector& pos)
{
   xw= pos.x;
   yw= pos.y;
   zw= pos.z;
}


// return the translation part of this matrix
Vector Matrix::translation() const
{
   Vector n;
   n.x= xw;
   n.y= yw;
   n.z= zw;
   return n;
}

Matrix Matrix::invert() const
{
   Matrix n;

   float det= xx*(yy*zz - yz*zy) + xy*(yz*zx - yx*zz) + xz*(yx*zy - yy*zx);

   // matrix not invertible?
   //   if (det==0) return ::identity();

   float inv= 1.0f / det;

   // generate adjoint matrix
   n.xx= (yy*zz - yz*zy)*inv;
   n.yx= (yz*zx - yx*zz)*inv;
   n.zx= (yx*zy - yy*zx)*inv;
   n.wx= 0.0f;

   n.xy= (xz*zy - xy*zz)*inv;
   n.yy= (xx*zz - xz*zx)*inv;
   n.zy= (xy*zx - xx*zy)*inv;
   n.wy= 0.0f;

   n.xz= (xy*yz - xz*yy)*inv;
   n.yz= (xz*yx - xx*yz)*inv;
   n.zz= (xx*yy - xy*yx)*inv;
   n.wz= 0.0f;

   // new translation vector = negative transposed rot/scale mat *  old translation vector
   n.xw = -(n.xx*xw) - (n.xy*yw) - (n.xz*zw);
   n.yw = -(n.yx*xw) - (n.yy*yw) - (n.yz*zw);
   n.zw = -(n.zx*xw) - (n.zy*yw) - (n.zz*zw);
   n.ww= 1.0f;

   return n;
}

Matrix Matrix::invert4x4() const
{
   Matrix n;

   float *inv= n.data();
   const float* m= this->data();

   inv[0] =   m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15]
            + m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
   inv[4] =  -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15]
            - m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
   inv[8] =   m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15]
            + m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
   inv[12] = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14]
            - m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];
   inv[1] =  -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15]
            - m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
   inv[5] =   m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15]
            + m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
   inv[9] =  -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15]
            - m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
   inv[13] =  m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14]
            + m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
   inv[2] =   m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15]
            + m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
   inv[6] =  -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15]
            - m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
   inv[10] =  m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15]
            + m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
   inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14]
            - m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
   inv[3] =  -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11]
            - m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
   inv[7] =   m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11]
            + m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
   inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11]
            - m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];
   inv[15] =  m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10]
            + m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];

   double det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];
   det = 1.0 / det;

   for (int i = 0; i < 16; i++)
       inv[i] = static_cast<float>((inv[i] * det));

   return n;
}

float* Matrix::data() const
{
   return const_cast<float*>(&xx);
}


void Matrix::print() const
{
   printf("%f %f %f %f \n", xx, xy, xz, xw);
   printf("%f %f %f %f \n", yx, yy, yz, yw);
   printf("%f %f %f %f \n", zx, zy, zz, zw);
   printf("%f %f %f %f \n", wx, wy, wz, ww);
   printf("\n");
}

Matrix Matrix::blend(const Matrix& m1, const Matrix& m2, float t)
{
   Matrix n;
   float it= 1.0f - t;
   n.xx= m1.xx * it + m2.xx * t;
   n.xy= m1.xy * it + m2.xy * t;
   n.xz= m1.xz * it + m2.xz * t;
   n.xw= m1.xw * it + m2.xw * t;
   n.yx= m1.yx * it + m2.yx * t;
   n.yy= m1.yy * it + m2.yy * t;
   n.yz= m1.yz * it + m2.yz * t;
   n.yw= m1.yw * it + m2.yw * t;
   n.zx= m1.zx * it + m2.zx * t;
   n.zy= m1.zy * it + m2.zy * t;
   n.zz= m1.zz * it + m2.zz * t;
   n.zw= m1.zw * it + m2.zw * t;
   n.wx= m1.wx * it + m2.wx * t;
   n.wy= m1.wy * it + m2.wy * t;
   n.wz= m1.wz * it + m2.wz * t;
   n.ww= m1.ww * it + m2.ww * t;
   return n;
}

Matrix Matrix::rotateX(const float f)
{
   Matrix m;

   float c = static_cast<float>(cosf(f));
   float s = static_cast<float>(sinf(f));

   m.xx = 1; m.xy = 0; m.xz =  0; m.xw = 0;
   m.yx = 0; m.yy = c; m.yz =  s; m.yw = 0;
   m.zx = 0; m.zy =-s; m.zz =  c; m.zw = 0;
   m.wx = 0; m.wy = 0; m.wz =  0; m.ww = 1;

   return m;
}

Matrix Matrix::rotateY(const float f)
{
   Matrix m;

   float c = static_cast<float>(cosf(f));
   float s = static_cast<float>(sinf(f));

   m.xx =  c; m.xy = 0; m.xz = s; m.xw = 0;
   m.yx =  0; m.yy = 1; m.yz = 0; m.yw = 0;
   m.zx = -s; m.zy = 0; m.zz = c; m.zw = 0;
   m.wx =  0; m.wy = 0; m.wz = 0; m.ww = 1;

   return m;
}

Matrix Matrix::rotateZ(const float f)
{
   Matrix m;

   float c = static_cast<float>(cosf(f));
   float s = static_cast<float>(sinf(f));

   m.xx = c;  m.xy =-s; m.xz = 0; m.xw = 0;
   m.yx = s;  m.yy = c; m.yz = 0; m.yw = 0;
   m.zx = 0;  m.zy = 0; m.zz = 1; m.zw = 0;
   m.wx = 0;  m.wy = 0; m.wz = 0; m.ww = 1;

   return m;
}

float Matrix::norm() const
{
   float x= fabs(xx) + fabs(yx) + fabs(zx);
   float y= fabs(xy) + fabs(yy) + fabs(zy);
   float z= fabs(xz) + fabs(yz) + fabs(zz);
   if (y>x) x=y;
   if (z>x) x=z;
   return x;
}

Vector Matrix::x() const
{
   return Vector(xx, xy, xz);
}

Vector Matrix::y() const
{
   return Vector(yx, yy, yz);
}

Vector Matrix::z() const
{
   return Vector(zx, zy, zz);
}


Matrix Matrix::adjointTranspose() const
{
   Matrix n;
/*
   n.xx= (yy*zz - yz*zy);
   n.yx= (yz*zx - yx*zz);
   n.zx= (yx*zy - yy*zx);
   n.wx = 0.0f;

   n.xy= (xz*zy - xy*zz);
   n.yy= (xx*zz - xz*zx);
   n.zy= (xy*zx - xx*zy);
   n.wy = 0.0f;

   n.xz= (xy*yz - xz*yy);
   n.yz= (xz*yx - xx*yz);
   n.zz= (xx*yy - xy*yx);
   n.wz = 0.0f;
*/

   n.xx= (yy*zz - yz*zy);
   n.xy= (yz*zx - yx*zz);
   n.xz= (yx*zy - yy*zx);
   n.xw = 0.0f;

   n.yx= (xz*zy - xy*zz);
   n.yy= (xx*zz - xz*zx);
   n.yz= (xy*zx - xx*zy);
   n.yw = 0.0f;

   n.zx= (xy*yz - xz*yy);
   n.zy= (xz*yx - xx*yz);
   n.zz= (xx*yy - xy*yx);
   n.zw = 0.0f;

   n.wx= 0.0f;
   n.wy= 0.0f;
   n.wz= 0.0f;
   n.ww= 1.0f;

   return n;
}

float Matrix::polarDecompose(Matrix& Q, Matrix& S) const
{
   #define TOL 1.0e-6
   float det, E_one;

   Matrix Mk= get3x3().transpose3x3();
   float M_one = norm();
   float M_inf = Mk.norm();
   do {
      Matrix MadjTk= Mk.adjointTranspose();
      det = Mk.x() * MadjTk.x();
      if (det==0.0)
      {
//         do_rank2(Mk, MadjTk, Mk);
         break;
      }
      float MadjT_one = MadjTk.norm();
      float MadjT_inf = MadjTk.transpose3x3().norm();
      float gamma = sqrt(sqrt((MadjT_one*MadjT_inf)/(M_one*M_inf))/fabs(det));
      float g1 = gamma*0.5f;
      float g2 = 0.5f/(gamma*det);
      Matrix Ek= Mk;
      Mk= Mk*g1 + MadjTk*g2;
      Ek= Ek-Mk;
      E_one = Ek.norm();
      M_one = Mk.transpose3x3().norm();
      M_inf = Mk.norm();
   } while (E_one > (M_one*TOL));

   Q= Mk;//.transpose3x3();
   S= Mk * get3x3();

   // unify 3x3
   S.yx= S.xy= (S.xy+S.yx)*0.5f;
   S.zx= S.xz= (S.xz+S.zx)*0.5f;
   S.zy= S.yz= (S.yz+S.zy)*0.5f;

   return det;
}

void Matrix::affineDecompose(Quat& q, Vector& pos, float& flip, Scale& scale) const
{
//   void decomp_affine(HMatrix A, AffineParts *parts)
   Matrix Q, S, U;
   Quat p;

   pos = Vector(xw, yw, zw);
   float det = polarDecompose(Q, S);

   if (det<0.0)
   {
      Q= Q * -1.0f;
      flip = -1.0f;
   }
   else
      flip= 1.0f;

   q = Quat(Q.get3x3());
   Vector sv = S.spectralDecompose(U);
   Quat u = Quat(U);
   p = u.snuggle(sv);

   scale= Scale(u * p, sv);
}

/*
Compute the spectral decomposition of symmetric positive semi-definite matrix
Returns rotation in U and scale factors in result, so that if K is a diagonal
matrix of the scale factors, then S = U K (U transpose). Uses Jacobi method.
*/
Vector Matrix::spectralDecompose(Matrix& U) const
{
   double Diag[3],OffD[3];
   double g,h,fabsh,t,theta,c,s,tau,ta,OffDq,a,b;

   static char nxt[] = { 1,2,0 };

   U.identity();

   Diag[0] = xx;
   Diag[1] = yy;
   Diag[2] = zz;

   OffD[0] = yz;
   OffD[1] = zx;
   OffD[2] = xy;

   for (int sweep=20; sweep>0; sweep--)
   {
      float sm = static_cast<float>(fabs(OffD[0])+fabs(OffD[1])+fabs(OffD[2]));
      if (sm==0.0) break;

      for (int i=2; i>=0; i--)
      {
         int p = nxt[i];
         int q = nxt[p];

         double fabsOffDi = fabs(OffD[i]);
         g = 100.0*fabsOffDi;
         if (fabsOffDi>0.0)
         {
            h = Diag[q] - Diag[p];
            fabsh = fabs(h);
            if (fabsh+g==fabsh)
            {
               t = OffD[i]/h;
            }
            else
            {
               theta = 0.5*h/OffD[i];
               t = 1.0/(fabs(theta)+sqrt(theta*theta+1.0));
               if (theta<0.0) t = -t;
            }
            c = 1.0/sqrt(t*t+1.0);
            s = t*c;
            tau = s/(c+1.0);
            ta = t*OffD[i];
            OffD[i] = 0.0;
            Diag[p] -= ta;
            Diag[q] += ta;
            OffDq = OffD[q];
            OffD[q] -= s*(OffD[p] + tau*OffD[q]);
            OffD[p] += s*(OffDq   - tau*OffD[p]);

            for (int j=2; j>=0; j--)
            {
               float *mat= static_cast<float*>(&U.xx + j*4);
               a = mat[p];
               b = mat[q];
               mat[p] -= static_cast<float>(s*(b + tau*a));
               mat[q] += static_cast<float>(s*(a - tau*b));
            }
         }
      }
   }

   return Vector(
      static_cast<float>(Diag[0]),
      static_cast<float>(Diag[1]),
      static_cast<float>(Diag[2])
   );
}


