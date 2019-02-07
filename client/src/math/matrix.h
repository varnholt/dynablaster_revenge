// simple matrix class

#pragma once

#include "vector.h"
#include "vector4.h"

class Stream;
class Scale;
class Quat;

class Matrix
{
public:
  float xx, yx, zx, wx,
        xy, yy, zy, wy,
        xz, yz, zz, wz,
        xw, yw, zw, ww;

  static Matrix rotateX(const float f);
  static Matrix rotateY(const float f);
  static Matrix rotateZ(const float f);
  static Matrix blend(const Matrix& m1, const Matrix& m2, float t);

  static Matrix scale(float sx, float sy, float sz);
  static Matrix position(float x, float y, float z);
  static Matrix position(const Vector& pos);
  static Matrix lookAt(const Vector& p, const Vector& t, const Vector& up= Vector(0,0,1));
  static Matrix frustum(float left, float right, float bottom, float top, float nearZ, float farZ);
  static Matrix ortho(float left, float right, float bottom, float top, float nearZ, float farZ);

  Matrix();
  Matrix(  float xx,        float yx,        float zx,        float wx,
           float xy,        float yy,        float zy,        float wy,
           float xz,        float yz,        float zz,        float wz,
           float xw = 0.0f, float yw = 0.0f, float zw = 0.0f, float ww = 1.0f );

  Matrix(const Vector& v);
  Matrix(const Vector& x, const Vector& y, const Vector& z);
  Matrix(const Quat& r);

  Vector operator * (const Vector& v) const;
  Vector4 operator * (const Vector4& v) const;
  Matrix operator * (const Matrix& m) const;
  Matrix operator + (const Matrix& m) const;
  Matrix operator - (const Matrix& m) const;
  Matrix operator * (const float s) const;

  operator const float* () const { return &xx; }

  Vector x() const;
  Vector y() const;
  Vector z() const;
  Matrix xyw() const;

  float  norm() const;                 // calculate 1-norm
  Vector translation() const;          // get translation-vector from matrix components
  void   translate(const Vector& pos); // set translation components
  void   identity();                   // set identity

  Matrix invert() const;
  Matrix invert4x4() const;
  Matrix get3x3() const;
  Matrix normalized() const;
  void normalizeZ();
  Matrix transpose() const;
  Matrix transpose3x3() const;
  Matrix getView() const;
  float* data() const;
  
  void   print() const;
  void   load(Stream *stream);
  void   write(Stream *stream);

  float  polarDecompose(Matrix& Q, Matrix& S) const;
  Matrix adjointTranspose() const;
  void   affineDecompose(Quat& t, Vector& pos, float& flip, Scale& sc) const;
  Vector spectralDecompose(Matrix& U) const;
};

