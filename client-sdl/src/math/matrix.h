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
   float xx, yx, zx, wx, xy, yy, zy, wy, xz, yz, zz, wz, xw, yw, zw, ww;

   static Matrix rotateX(const float angle_radians);
   static Matrix rotateY(const float angle_radians);
   static Matrix rotateZ(const float angle_radians);
   static Matrix blend(const Matrix& start, const Matrix& end, float blend_factor);

   static Matrix scale(float sx, float sy, float sz);
   static Matrix position(float x, float y, float z);
   static Matrix position(const Vector& position);
   static Matrix lookAt(const Vector& eye, const Vector& target, const Vector& up = Vector(0, 0, 1));
   static Matrix frustum(float left, float right, float bottom, float top, float nearZ, float farZ);
   static Matrix ortho(float left, float right, float bottom, float top, float nearZ, float farZ);

   Matrix();
   Matrix(
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
      float xw = 0.0f,
      float yw = 0.0f,
      float zw = 0.0f,
      float ww = 1.0f
   );

   Matrix(const Vector& other);
   Matrix(const Vector& x, const Vector& y, const Vector& z);
   Matrix(const Quat& rotation);

   Vector operator*(const Vector& other) const;
   Vector4 operator*(const Vector4& other) const;
   Matrix operator*(const Matrix& other) const;
   Matrix operator+(const Matrix& other) const;
   Matrix operator-(const Matrix& other) const;
   Matrix operator*(const float scalar) const;

   operator const float*() const
   {
      return &xx;
   }

   Vector x() const;
   Vector y() const;
   Vector z() const;
   Matrix xyw() const;

   float norm() const;                      // calculate 1-norm
   Vector translation() const;              // get translation-vector from matrix components
   void translate(const Vector& position);  // set translation components
   void identity();                         // set identity

   Matrix invert() const;
   Matrix invert4x4() const;
   Matrix get3x3() const;
   Matrix normalized() const;
   void normalizeZ();
   Matrix transpose() const;
   Matrix transpose3x3() const;
   Matrix getView() const;
   float* data() const;

   void print() const;
   void load(Stream* stream);
   void write(Stream* stream);

   // The remaining trio (polarDecompose/affineDecompose/spectralDecompose) implements Ken
   // Shoemake's published matrix-decomposition algorithm ("Polar Matrix Decomposition", Graphics
   // Gems IV) - their parameter/local names intentionally keep that paper's own notation (Q, S,
   // U, ...) rather than being renamed to fully descriptive names, since misnaming a step of a
   // dense numerical algorithm like this is a real correctness risk that outweighs the
   // readability gain a rename here would give.
   float polarDecompose(Matrix& Q, Matrix& S) const;
   Matrix adjointTranspose() const;
   void affineDecompose(Quat& rotation, Vector& position, float& flip, Scale& scale) const;
   Vector spectralDecompose(Matrix& U) const;
};
