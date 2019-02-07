#pragma once

class Detonation
{
public: 
   Detonation(int cx, int cy, int left, int right, int top, int bottom);

   int getX() const;
   int getY() const;
   int getUp() const;
   int getDown() const;
   int getLeft() const;
   int getRight() const;

   void setStartTime(float time);
   float elapsed(float time) const;

private:
   int mCenterX;
   int mCenterY;
   int mLeft;
   int mRight;
   int mTop;
   int mBottom;
   float mStartTime;
};