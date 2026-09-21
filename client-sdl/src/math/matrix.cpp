#include "matrix.h"
#include <math.h>
#include <stdio.h>
#include "scale.h"
#include "tools/stream.h"

Matrix::Matrix()
    : xx(1.0f),
      yx(0.0f),
      zx(0.0f),
      wx(0.0f),
      xy(0.0f),
      yy(1.0f),
      zy(0.0f),
      wy(0.0f),
      xz(0.0f),
      yz(0.0f),
      zz(1.0f),
      wz(0.0f),
      xw(0.0f),
      yw(0.0f),
      zw(0.0f),
      ww(1.0f)
{
}

Matrix::Matrix(
   float xx,
   float yx,
   float zx,
   float wx,
   float xy,
   float yy,
   float zy,
   float wy,
   float xz,
   float yz,
   float zz,
   float wz,
   float xw,
   float yw,
   float zw,
   float ww
)
    : xx(xx), yx(yx), zx(zx), wx(wx), xy(xy), yy(yy), zy(zy), wy(wy), xz(xz), yz(yz), zz(zz), wz(wz), xw(xw), yw(yw), zw(zw), ww(ww)
{
}

Matrix::Matrix(const Vector& other)
{
   xx = other.x;
   xy = 0.0f;
   xz = 0.0f;
   xw = 0.0f;
   yx = 0.0f;
   yy = other.y;
   yz = 0.0f;
   yw = 0.0f;
   zx = 0.0f;
   zy = 0.0f;
   zz = other.z;
   zw = 0.0f;
   wx = 0.0f;
   wy = 0.0f;
   wz = 0.0f;
   ww = 1.0f;
}

Matrix::Matrix(const Vector& x, const Vector& y, const Vector& z)
{
   xx = x.x;
   xy = y.x;
   xz = z.x;
   xw = 0.0f;
   yx = x.y;
   yy = y.y;
   yz = z.y;
   yw = 0.0f;
   zx = x.z;
   zy = y.z;
   zz = z.z;
   zw = 0.0f;
   wx = 0.0f;
   wy = 0.0f;
   wz = 0.0f;
   ww = 1.0f;
}

Matrix::Matrix(const Quat& rotation)
{
   float r2xx = rotation.x * rotation.x * 2.0f;
   float r2yy = rotation.y * rotation.y * 2.0f;
   float r2zz = rotation.z * rotation.z * 2.0f;
   float r2xy = rotation.x * rotation.y * 2.0f;
   float r2xz = rotation.x * rotation.z * 2.0f;
   float r2yz = rotation.y * rotation.z * 2.0f;
   float r2wx = rotation.w * rotation.x * 2.0f;
   float r2wy = rotation.w * rotation.y * 2.0f;
   float r2wz = rotation.w * rotation.z * 2.0f;

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

void Matrix::load(Stream* stream)
{
   xx = stream->getFloat();
   yx = stream->getFloat();
   zx = stream->getFloat();
   wx = stream->getFloat();
   xy = stream->getFloat();
   yy = stream->getFloat();
   zy = stream->getFloat();
   wy = stream->getFloat();
   xz = stream->getFloat();
   yz = stream->getFloat();
   zz = stream->getFloat();
   wz = stream->getFloat();
   xw = stream->getFloat();
   yw = stream->getFloat();
   zw = stream->getFloat();
   ww = stream->getFloat();
}

void Matrix::write(Stream* stream)
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

Matrix Matrix::frustum(float left, float right, float bottom, float top, float near_z, float far_z)
{
   Matrix result;
   float inv_rl = 1.0f / (right - left);
   float inv_tb = 1.0f / (top - bottom);
   float inv_fn = 1.0f / (far_z - near_z);
   result.xx = 2.0f * near_z * inv_rl;
   result.xy = 0.0f;
   result.xz = (right + left) * inv_rl;
   result.xw = 0.0f;
   result.yx = 0.0f;
   result.yy = 2.0f * near_z * inv_tb;
   result.yz = (top + bottom) * inv_tb;
   result.yw = 0.0f;
   result.zx = 0.0f;
   result.zy = 0.0f;
   result.zz = -(far_z + near_z) * inv_fn;
   result.zw = -2.0f * far_z * near_z * inv_fn;
   result.wx = 0.0f;
   result.wy = 0.0f;
   result.wz = -1.0f;
   result.ww = 0.0f;
   return result;
}

Matrix Matrix::ortho(float left, float right, float bottom, float top, float near_z, float far_z)
{
   Matrix result;
   float inv_rl = 1.0f / (right - left);
   float inv_tb = 1.0f / (top - bottom);
   float inv_fn = 1.0f / (far_z - near_z);
   result.xx = 2.0f * inv_rl;
   result.xy = 0.0f;
   result.xz = 0.0f;
   result.xw = -(right + left) * inv_rl;
   result.yx = 0.0f;
   result.yy = 2.0f * inv_tb;
   result.yz = 0.0f;
   result.yw = -(top + bottom) * inv_tb;
   result.zx = 0.0f;
   result.zy = 0.0f;
   result.zz = -2.0f * inv_fn;
   result.zw = -(far_z + near_z) * inv_fn;
   result.wx = 0.0f;
   result.wy = 0.0f;
   result.wz = 0.0f;
   result.ww = 1.0f;
   return result;
}

//! creates look-at matrix compatible to gluLookAt
//! up vector must be normalized
Matrix Matrix::lookAt(const Vector& eye, const Vector& target, const Vector& up)
{
   Vector z = (eye - target);
   z.normalize();

   Vector x = (-z % up);
   Vector y = (x % -z);

   Matrix result;
   result.xx = x.x;
   result.xy = x.y;
   result.xz = x.z;
   result.xw = -result.xx * eye.x - result.xy * eye.y - result.xz * eye.z;

   result.yx = y.x;
   result.yy = y.y;
   result.yz = y.z;
   result.yw = -result.yx * eye.x - result.yy * eye.y - result.yz * eye.z;

   result.zx = z.x;
   result.zy = z.y;
   result.zz = z.z;
   result.zw = -result.zx * eye.x - result.zy * eye.y - result.zz * eye.z;

   result.wx = 0.0f;
   result.wy = 0.0f;
   result.wz = 0.0f;
   result.ww = 1.0f;

   return result;
}

Matrix Matrix::scale(float sx, float sy, float sz)
{
   Matrix result;
   result.xx = sx;
   result.xy = 0.0f;
   result.xz = 0.0f;
   result.xw = 0.0f;
   result.yx = 0.0f;
   result.yy = sy;
   result.yz = 0.0f;
   result.yw = 0.0f;
   result.zx = 0.0f;
   result.zy = 0.0f;
   result.zz = sz;
   result.zw = 0.0f;
   result.wx = 0.0f;
   result.wy = 0.0f;
   result.wz = 0.0f;
   result.ww = 1.0f;
   return result;
}

Matrix Matrix::position(const Vector& position)
{
   Matrix result;
   result.xx = 1.0f;
   result.xy = 0.0f;
   result.xz = 0.0f;
   result.xw = position.x;
   result.yx = 0.0f;
   result.yy = 1.0f;
   result.yz = 0.0f;
   result.yw = position.y;
   result.zx = 0.0f;
   result.zy = 0.0f;
   result.zz = 1.0f;
   result.zw = position.z;
   result.wx = 0.0f;
   result.wy = 0.0f;
   result.wz = 0.0f;
   result.ww = 1.0f;
   return result;
}

Matrix Matrix::position(float x, float y, float z)
{
   Matrix result;
   result.xx = 1.0f;
   result.xy = 0.0f;
   result.xz = 0.0f;
   result.xw = x;
   result.yx = 0.0f;
   result.yy = 1.0f;
   result.yz = 0.0f;
   result.yw = y;
   result.zx = 0.0f;
   result.zy = 0.0f;
   result.zz = 1.0f;
   result.zw = z;
   result.wx = 0.0f;
   result.wy = 0.0f;
   result.wz = 0.0f;
   result.ww = 1.0f;
   return result;
}

// vector= matrix * vector
Vector Matrix::operator*(const Vector& other) const
{
   Vector result;

   result.x = xx * other.x + xy * other.y + xz * other.z + xw;
   result.y = yx * other.x + yy * other.y + yz * other.z + yw;
   result.z = zx * other.x + zy * other.y + zz * other.z + zw;

   return result;
}

// vector= matrix * vector
Vector4 Matrix::operator*(const Vector4& other) const
{
   Vector4 result;

   result.x = xx * other.x + xy * other.y + xz * other.z + xw * other.w;
   result.y = yx * other.x + yy * other.y + yz * other.z + yw * other.w;
   result.z = zx * other.x + zy * other.y + zz * other.z + zw * other.w;
   result.w = wx * other.x + wy * other.y + wz * other.z + ww * other.w;

   return result;
}

// matrix * scalar
Matrix Matrix::operator*(const float scalar) const
{
   Matrix result;

   result.xx = xx * scalar;
   result.xy = xy * scalar;
   result.xz = xz * scalar;
   result.xw = xw * scalar;
   result.yx = yx * scalar;
   result.yy = yy * scalar;
   result.yz = yz * scalar;
   result.yw = yw * scalar;
   result.zx = zx * scalar;
   result.zy = zy * scalar;
   result.zz = zz * scalar;
   result.zw = zw * scalar;
   result.wx = wx * scalar;
   result.wy = wy * scalar;
   result.wz = wz * scalar;
   result.ww = ww * scalar;

   return result;
}

// matrix + matrix
Matrix Matrix::operator+(const Matrix& other) const
{
   Matrix result;

   result.xx = other.xx + xx;
   result.xy = other.xy + xy;
   result.xz = other.xz + xz;
   result.xw = other.xw + xw;
   result.yx = other.yx + yx;
   result.yy = other.yy + yy;
   result.yz = other.yz + yz;
   result.yw = other.yw + yw;
   result.zx = other.zx + zx;
   result.zy = other.zy + zy;
   result.zz = other.zz + zz;
   result.zw = other.zw + zw;
   result.wx = other.wx + wx;
   result.wy = other.wy + wy;
   result.wz = other.wz + wz;
   result.ww = other.ww + ww;

   return result;
}

// matrix - matrix
Matrix Matrix::operator-(const Matrix& other) const
{
   Matrix result;

   result.xx = xx - other.xx;
   result.xy = xy - other.xy;
   result.xz = xz - other.xz;
   result.xw = xw - other.xw;
   result.yx = yx - other.yx;
   result.yy = yy - other.yy;
   result.yz = yz - other.yz;
   result.yw = yw - other.yw;
   result.zx = zx - other.zx;
   result.zy = zy - other.zy;
   result.zz = zz - other.zz;
   result.zw = zw - other.zw;
   result.wx = wx - other.wx;
   result.wy = wy - other.wy;
   result.wz = wz - other.wz;
   result.ww = ww - other.ww;

   return result;
}

Matrix Matrix::operator*(const Matrix& other) const
{
   Matrix result;

   result.xx = other.xx * xx + other.xy * yx + other.xz * zx + other.xw * wx;
   result.xy = other.xx * xy + other.xy * yy + other.xz * zy + other.xw * wy;
   result.xz = other.xx * xz + other.xy * yz + other.xz * zz + other.xw * wz;
   result.xw = other.xx * xw + other.xy * yw + other.xz * zw + other.xw * ww;

   result.yx = other.yx * xx + other.yy * yx + other.yz * zx + other.yw * wx;
   result.yy = other.yx * xy + other.yy * yy + other.yz * zy + other.yw * wy;
   result.yz = other.yx * xz + other.yy * yz + other.yz * zz + other.yw * wz;
   result.yw = other.yx * xw + other.yy * yw + other.yz * zw + other.yw * ww;

   result.zx = other.zx * xx + other.zy * yx + other.zz * zx + other.zw * wx;
   result.zy = other.zx * xy + other.zy * yy + other.zz * zy + other.zw * wy;
   result.zz = other.zx * xz + other.zy * yz + other.zz * zz + other.zw * wz;
   result.zw = other.zx * xw + other.zy * yw + other.zz * zw + other.zw * ww;

   result.wx = other.wx * xx + other.wy * yx + other.wz * zx + other.ww * wx;
   result.wy = other.wx * xy + other.wy * yy + other.wz * zy + other.ww * wy;
   result.wz = other.wx * xz + other.wy * yz + other.wz * zz + other.ww * wz;
   result.ww = other.wx * xw + other.wy * yw + other.wz * zw + other.ww * ww;

   return result;
}

void Matrix::identity()
{
   xx = 1.0f;
   xy = 0.0f;
   xz = 0.0f;
   xw = 0.0f;
   yx = 0.0f;
   yy = 1.0f;
   yz = 0.0f;
   yw = 0.0f;
   zx = 0.0f;
   zy = 0.0f;
   zz = 1.0f;
   zw = 0.0f;
   wx = 0.0f;
   wy = 0.0f;
   wz = 0.0f;
   ww = 1.0f;
}

Matrix Matrix::getView() const
{
   Vector negated_translation = -translation();
   Matrix result = transpose3x3();

   result.xw = xx * negated_translation.x + yx * negated_translation.y + zx * negated_translation.z;
   result.yw = xy * negated_translation.x + yy * negated_translation.y + zy * negated_translation.z;
   result.zw = xz * negated_translation.x + yz * negated_translation.y + zz * negated_translation.z;

   return result;
}

// return xyw columns as new matrix
Matrix Matrix::xyw() const
{
   Matrix result;

   result.xx = xx;
   result.xy = xy;
   result.xz = xz;
   result.xw = xw;
   result.yx = yx;
   result.yy = yy;
   result.yz = yz;
   result.yw = yw;
   result.zx = wx;
   result.zy = wy;
   result.zz = wz;
   result.zw = ww;
   result.wx = 0.0f;
   result.wy = 0.0f;
   result.wz = 0.0f;
   result.ww = 1.0f;

   return result;
}

Matrix Matrix::normalized() const
{
   Matrix result;
   float scale_factor, magnitude_squared;

   scale_factor = 1.0f / sqrtf(xx * xx + xy * xy + xz * xz);
   result.xx = xx * scale_factor;
   result.xy = xy * scale_factor;
   result.xz = xz * scale_factor;
   result.xw = xw;

   scale_factor = 1.0f / sqrtf(yx * yx + yy * yy + yz * yz);
   result.yx = yx * scale_factor;
   result.yy = yy * scale_factor;
   result.yz = yz * scale_factor;
   result.yw = yw;

   scale_factor = 1.0f / sqrtf(zx * zx + zy * zy + zz * zz);
   result.zx = zx * scale_factor;
   result.zy = zy * scale_factor;
   result.zz = zz * scale_factor;
   result.zw = zw;

   magnitude_squared = wx * wx + wy * wy + wz * wz;
   if (magnitude_squared > 0.00000001f)
      scale_factor = 1.0f / sqrtf(magnitude_squared);
   else
      scale_factor = 0.0f;

   result.wx = wx * scale_factor;
   result.wy = wy * scale_factor;
   result.wz = wz * scale_factor;
   result.ww = ww;

   return result;
}

void Matrix::normalizeZ()
{
   float scale_factor = 1.0f / sqrt(zx * zx + zy * zy + zz * zz);
   zx *= scale_factor;
   zy *= scale_factor;
   zz *= scale_factor;
}

Matrix Matrix::get3x3() const
{
   Matrix result;
   result.xx = xx;
   result.xy = xy;
   result.xz = xz;
   result.xw = 0.0f;

   result.yx = yx;
   result.yy = yy;
   result.yz = yz;
   result.yw = 0.0f;

   result.zx = zx;
   result.zy = zy;
   result.zz = zz;
   result.zw = 0.0f;

   result.wx = 0.0f;
   result.wy = 0.0f;
   result.wz = 0.0f;
   result.ww = 1.0f;

   return result;
}

Matrix Matrix::transpose() const
{
   Matrix result;

   result.xx = xx;
   result.xy = yx;
   result.xz = zx;
   result.xw = wx;

   result.yx = xy;
   result.yy = yy;
   result.yz = zy;
   result.yw = wy;

   result.zx = xz;
   result.zy = yz;
   result.zz = zz;
   result.zw = wz;

   result.wx = xw;  // position.x
   result.wy = yw;  // position.y
   result.wz = zw;  // position.z
   result.ww = ww;  // 1.0

   return result;
}

Matrix Matrix::transpose3x3() const
{
   Matrix result;

   result.xx = xx;
   result.xy = yx;
   result.xz = zx;
   result.xw = xw;  // position.x

   result.yx = xy;
   result.yy = yy;
   result.yz = zy;
   result.yw = yw;  // position.y

   result.zx = xz;
   result.zy = yz;
   result.zz = zz;
   result.zw = zw;  // position.z

   result.wx = wx;
   result.wy = wy;
   result.wz = wz;
   result.ww = ww;  // 1.0

   return result;
}

void Matrix::translate(const Vector& position)
{
   xw = position.x;
   yw = position.y;
   zw = position.z;
}

// return the translation part of this matrix
Vector Matrix::translation() const
{
   Vector result;
   result.x = xw;
   result.y = yw;
   result.z = zw;
   return result;
}

Matrix Matrix::invert() const
{
   Matrix result;

   float det = xx * (yy * zz - yz * zy) + xy * (yz * zx - yx * zz) + xz * (yx * zy - yy * zx);

   // matrix not invertible?
   //   if (det==0) return ::identity();

   float inv = 1.0f / det;

   // generate adjoint matrix
   result.xx = (yy * zz - yz * zy) * inv;
   result.yx = (yz * zx - yx * zz) * inv;
   result.zx = (yx * zy - yy * zx) * inv;
   result.wx = 0.0f;

   result.xy = (xz * zy - xy * zz) * inv;
   result.yy = (xx * zz - xz * zx) * inv;
   result.zy = (xy * zx - xx * zy) * inv;
   result.wy = 0.0f;

   result.xz = (xy * yz - xz * yy) * inv;
   result.yz = (xz * yx - xx * yz) * inv;
   result.zz = (xx * yy - xy * yx) * inv;
   result.wz = 0.0f;

   // new translation vector = negative transposed rot/scale mat *  old translation vector
   result.xw = -(result.xx * xw) - (result.xy * yw) - (result.xz * zw);
   result.yw = -(result.yx * xw) - (result.yy * yw) - (result.yz * zw);
   result.zw = -(result.zx * xw) - (result.zy * yw) - (result.zz * zw);
   result.ww = 1.0f;

   return result;
}

Matrix Matrix::invert4x4() const
{
   Matrix result;

   float* inv = result.data();
   const float* mat = this->data();

   inv[0] = mat[5] * mat[10] * mat[15] - mat[5] * mat[11] * mat[14] - mat[9] * mat[6] * mat[15] + mat[9] * mat[7] * mat[14] +
            mat[13] * mat[6] * mat[11] - mat[13] * mat[7] * mat[10];
   inv[4] = -mat[4] * mat[10] * mat[15] + mat[4] * mat[11] * mat[14] + mat[8] * mat[6] * mat[15] - mat[8] * mat[7] * mat[14] -
            mat[12] * mat[6] * mat[11] + mat[12] * mat[7] * mat[10];
   inv[8] = mat[4] * mat[9] * mat[15] - mat[4] * mat[11] * mat[13] - mat[8] * mat[5] * mat[15] + mat[8] * mat[7] * mat[13] +
            mat[12] * mat[5] * mat[11] - mat[12] * mat[7] * mat[9];
   inv[12] = -mat[4] * mat[9] * mat[14] + mat[4] * mat[10] * mat[13] + mat[8] * mat[5] * mat[14] - mat[8] * mat[6] * mat[13] -
             mat[12] * mat[5] * mat[10] + mat[12] * mat[6] * mat[9];
   inv[1] = -mat[1] * mat[10] * mat[15] + mat[1] * mat[11] * mat[14] + mat[9] * mat[2] * mat[15] - mat[9] * mat[3] * mat[14] -
            mat[13] * mat[2] * mat[11] + mat[13] * mat[3] * mat[10];
   inv[5] = mat[0] * mat[10] * mat[15] - mat[0] * mat[11] * mat[14] - mat[8] * mat[2] * mat[15] + mat[8] * mat[3] * mat[14] +
            mat[12] * mat[2] * mat[11] - mat[12] * mat[3] * mat[10];
   inv[9] = -mat[0] * mat[9] * mat[15] + mat[0] * mat[11] * mat[13] + mat[8] * mat[1] * mat[15] - mat[8] * mat[3] * mat[13] -
            mat[12] * mat[1] * mat[11] + mat[12] * mat[3] * mat[9];
   inv[13] = mat[0] * mat[9] * mat[14] - mat[0] * mat[10] * mat[13] - mat[8] * mat[1] * mat[14] + mat[8] * mat[2] * mat[13] +
             mat[12] * mat[1] * mat[10] - mat[12] * mat[2] * mat[9];
   inv[2] = mat[1] * mat[6] * mat[15] - mat[1] * mat[7] * mat[14] - mat[5] * mat[2] * mat[15] + mat[5] * mat[3] * mat[14] +
            mat[13] * mat[2] * mat[7] - mat[13] * mat[3] * mat[6];
   inv[6] = -mat[0] * mat[6] * mat[15] + mat[0] * mat[7] * mat[14] + mat[4] * mat[2] * mat[15] - mat[4] * mat[3] * mat[14] -
            mat[12] * mat[2] * mat[7] + mat[12] * mat[3] * mat[6];
   inv[10] = mat[0] * mat[5] * mat[15] - mat[0] * mat[7] * mat[13] - mat[4] * mat[1] * mat[15] + mat[4] * mat[3] * mat[13] +
             mat[12] * mat[1] * mat[7] - mat[12] * mat[3] * mat[5];
   inv[14] = -mat[0] * mat[5] * mat[14] + mat[0] * mat[6] * mat[13] + mat[4] * mat[1] * mat[14] - mat[4] * mat[2] * mat[13] -
             mat[12] * mat[1] * mat[6] + mat[12] * mat[2] * mat[5];
   inv[3] = -mat[1] * mat[6] * mat[11] + mat[1] * mat[7] * mat[10] + mat[5] * mat[2] * mat[11] - mat[5] * mat[3] * mat[10] -
            mat[9] * mat[2] * mat[7] + mat[9] * mat[3] * mat[6];
   inv[7] = mat[0] * mat[6] * mat[11] - mat[0] * mat[7] * mat[10] - mat[4] * mat[2] * mat[11] + mat[4] * mat[3] * mat[10] +
            mat[8] * mat[2] * mat[7] - mat[8] * mat[3] * mat[6];
   inv[11] = -mat[0] * mat[5] * mat[11] + mat[0] * mat[7] * mat[9] + mat[4] * mat[1] * mat[11] - mat[4] * mat[3] * mat[9] -
             mat[8] * mat[1] * mat[7] + mat[8] * mat[3] * mat[5];
   inv[15] = mat[0] * mat[5] * mat[10] - mat[0] * mat[6] * mat[9] - mat[4] * mat[1] * mat[10] + mat[4] * mat[2] * mat[9] +
             mat[8] * mat[1] * mat[6] - mat[8] * mat[2] * mat[5];

   double det = mat[0] * inv[0] + mat[1] * inv[4] + mat[2] * inv[8] + mat[3] * inv[12];
   det = 1.0 / det;

   for (int i = 0; i < 16; i++)
      inv[i] = static_cast<float>((inv[i] * det));

   return result;
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

Matrix Matrix::blend(const Matrix& start, const Matrix& end, float blend_factor)
{
   Matrix result;
   float inverse_blend_factor = 1.0f - blend_factor;
   result.xx = start.xx * inverse_blend_factor + end.xx * blend_factor;
   result.xy = start.xy * inverse_blend_factor + end.xy * blend_factor;
   result.xz = start.xz * inverse_blend_factor + end.xz * blend_factor;
   result.xw = start.xw * inverse_blend_factor + end.xw * blend_factor;
   result.yx = start.yx * inverse_blend_factor + end.yx * blend_factor;
   result.yy = start.yy * inverse_blend_factor + end.yy * blend_factor;
   result.yz = start.yz * inverse_blend_factor + end.yz * blend_factor;
   result.yw = start.yw * inverse_blend_factor + end.yw * blend_factor;
   result.zx = start.zx * inverse_blend_factor + end.zx * blend_factor;
   result.zy = start.zy * inverse_blend_factor + end.zy * blend_factor;
   result.zz = start.zz * inverse_blend_factor + end.zz * blend_factor;
   result.zw = start.zw * inverse_blend_factor + end.zw * blend_factor;
   result.wx = start.wx * inverse_blend_factor + end.wx * blend_factor;
   result.wy = start.wy * inverse_blend_factor + end.wy * blend_factor;
   result.wz = start.wz * inverse_blend_factor + end.wz * blend_factor;
   result.ww = start.ww * inverse_blend_factor + end.ww * blend_factor;
   return result;
}

Matrix Matrix::rotateX(const float angle_radians)
{
   Matrix result;

   float cos_angle = static_cast<float>(cosf(angle_radians));
   float sin_angle = static_cast<float>(sinf(angle_radians));

   result.xx = 1;
   result.xy = 0;
   result.xz = 0;
   result.xw = 0;
   result.yx = 0;
   result.yy = cos_angle;
   result.yz = sin_angle;
   result.yw = 0;
   result.zx = 0;
   result.zy = -sin_angle;
   result.zz = cos_angle;
   result.zw = 0;
   result.wx = 0;
   result.wy = 0;
   result.wz = 0;
   result.ww = 1;

   return result;
}

Matrix Matrix::rotateY(const float angle_radians)
{
   Matrix result;

   float cos_angle = static_cast<float>(cosf(angle_radians));
   float sin_angle = static_cast<float>(sinf(angle_radians));

   result.xx = cos_angle;
   result.xy = 0;
   result.xz = sin_angle;
   result.xw = 0;
   result.yx = 0;
   result.yy = 1;
   result.yz = 0;
   result.yw = 0;
   result.zx = -sin_angle;
   result.zy = 0;
   result.zz = cos_angle;
   result.zw = 0;
   result.wx = 0;
   result.wy = 0;
   result.wz = 0;
   result.ww = 1;

   return result;
}

Matrix Matrix::rotateZ(const float angle_radians)
{
   Matrix result;

   float cos_angle = static_cast<float>(cosf(angle_radians));
   float sin_angle = static_cast<float>(sinf(angle_radians));

   result.xx = cos_angle;
   result.xy = -sin_angle;
   result.xz = 0;
   result.xw = 0;
   result.yx = sin_angle;
   result.yy = cos_angle;
   result.yz = 0;
   result.yw = 0;
   result.zx = 0;
   result.zy = 0;
   result.zz = 1;
   result.zw = 0;
   result.wx = 0;
   result.wy = 0;
   result.wz = 0;
   result.ww = 1;

   return result;
}

float Matrix::norm() const
{
   float norm_x = fabs(xx) + fabs(yx) + fabs(zx);
   float norm_y = fabs(xy) + fabs(yy) + fabs(zy);
   float norm_z = fabs(xz) + fabs(yz) + fabs(zz);
   if (norm_y > norm_x)
      norm_x = norm_y;
   if (norm_z > norm_x)
      norm_x = norm_z;
   return norm_x;
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
   Matrix result;

   result.xx = (yy * zz - yz * zy);
   result.xy = (yz * zx - yx * zz);
   result.xz = (yx * zy - yy * zx);
   result.xw = 0.0f;

   result.yx = (xz * zy - xy * zz);
   result.yy = (xx * zz - xz * zx);
   result.yz = (xy * zx - xx * zy);
   result.yw = 0.0f;

   result.zx = (xy * yz - xz * yy);
   result.zy = (xz * yx - xx * yz);
   result.zz = (xx * yy - xy * yx);
   result.zw = 0.0f;

   result.wx = 0.0f;
   result.wy = 0.0f;
   result.wz = 0.0f;
   result.ww = 1.0f;

   return result;
}

// The internals of polarDecompose/affineDecompose/spectralDecompose below intentionally keep Ken
// Shoemake's own notation from "Polar Matrix Decomposition" (Graphics Gems IV) rather than being
// renamed to fully descriptive names - see matrix.h's comment on why.

float Matrix::polarDecompose(Matrix& Q, Matrix& S) const
{
#define TOL 1.0e-6
   float det, E_one;

   Matrix Mk = get3x3().transpose3x3();
   float M_one = norm();
   float M_inf = Mk.norm();
   do
   {
      Matrix MadjTk = Mk.adjointTranspose();
      det = Mk.x() * MadjTk.x();
      if (det == 0.0)
      {
         //         do_rank2(Mk, MadjTk, Mk);
         break;
      }
      float MadjT_one = MadjTk.norm();
      float MadjT_inf = MadjTk.transpose3x3().norm();
      float gamma = sqrt(sqrt((MadjT_one * MadjT_inf) / (M_one * M_inf)) / fabs(det));
      float g1 = gamma * 0.5f;
      float g2 = 0.5f / (gamma * det);
      Matrix Ek = Mk;
      Mk = Mk * g1 + MadjTk * g2;
      Ek = Ek - Mk;
      E_one = Ek.norm();
      M_one = Mk.transpose3x3().norm();
      M_inf = Mk.norm();
   } while (E_one > (M_one * TOL));

   Q = Mk;  //.transpose3x3();
   S = Mk * get3x3();

   // unify 3x3
   S.yx = S.xy = (S.xy + S.yx) * 0.5f;
   S.zx = S.xz = (S.xz + S.zx) * 0.5f;
   S.zy = S.yz = (S.yz + S.zy) * 0.5f;

   return det;
}

void Matrix::affineDecompose(Quat& rotation, Vector& position, float& flip, Scale& scale) const
{
   //   void decomp_affine(HMatrix A, AffineParts *parts)
   Matrix Q, S, U;
   Quat p;

   position = Vector(xw, yw, zw);
   float det = polarDecompose(Q, S);

   if (det < 0.0)
   {
      Q = Q * -1.0f;
      flip = -1.0f;
   }
   else
      flip = 1.0f;

   rotation = Quat(Q.get3x3());
   Vector sv = S.spectralDecompose(U);
   Quat u = Quat(U);
   p = u.snuggle(sv);

   scale = Scale(u * p, sv);
}

/*
Compute the spectral decomposition of symmetric positive semi-definite matrix
Returns rotation in U and scale factors in result, so that if K is a diagonal
matrix of the scale factors, then S = U K (U transpose). Uses Jacobi method.
*/
Vector Matrix::spectralDecompose(Matrix& U) const
{
   double Diag[3], OffD[3];
   double g, h, fabsh, t, theta, c, s, tau, ta, OffDq, a, b;

   static char nxt[] = {1, 2, 0};

   U.identity();

   Diag[0] = xx;
   Diag[1] = yy;
   Diag[2] = zz;

   OffD[0] = yz;
   OffD[1] = zx;
   OffD[2] = xy;

   for (int sweep = 20; sweep > 0; sweep--)
   {
      float sm = static_cast<float>(fabs(OffD[0]) + fabs(OffD[1]) + fabs(OffD[2]));
      if (sm == 0.0)
         break;

      for (int i = 2; i >= 0; i--)
      {
         int p = nxt[i];
         int q = nxt[p];

         double fabsOffDi = fabs(OffD[i]);
         g = 100.0 * fabsOffDi;
         if (fabsOffDi > 0.0)
         {
            h = Diag[q] - Diag[p];
            fabsh = fabs(h);
            if (fabsh + g == fabsh)
            {
               t = OffD[i] / h;
            }
            else
            {
               theta = 0.5 * h / OffD[i];
               t = 1.0 / (fabs(theta) + sqrt(theta * theta + 1.0));
               if (theta < 0.0)
                  t = -t;
            }
            c = 1.0 / sqrt(t * t + 1.0);
            s = t * c;
            tau = s / (c + 1.0);
            ta = t * OffD[i];
            OffD[i] = 0.0;
            Diag[p] -= ta;
            Diag[q] += ta;
            OffDq = OffD[q];
            OffD[q] -= s * (OffD[p] + tau * OffD[q]);
            OffD[p] += s * (OffDq - tau * OffD[p]);

            for (int j = 2; j >= 0; j--)
            {
               float* mat = static_cast<float*>(&U.xx + j * 4);
               a = mat[p];
               b = mat[q];
               mat[p] -= static_cast<float>(s * (b + tau * a));
               mat[q] += static_cast<float>(s * (a - tau * b));
            }
         }
      }
   }

   return Vector(static_cast<float>(Diag[0]), static_cast<float>(Diag[1]), static_cast<float>(Diag[2]));
}
