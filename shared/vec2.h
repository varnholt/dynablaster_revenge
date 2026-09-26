#ifndef VEC2_H
#define VEC2_H


class Vec2
{

public:

   //! default constructor, zero-initialized
   Vec2();

   Vec2(float x, float y);

   float x() const;
   float y() const;

   void setX(float x);
   void setY(float y);

   float length() const;

   Vec2 operator-(const Vec2& other) const;


private:

   float mX;
   float mY;
};

#endif
