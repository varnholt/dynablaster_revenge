#ifndef JOYSTICKBALLVALUE_H
#define JOYSTICKBALLVALUE_H

class JoystickBallVector
{
   public:

      JoystickBallVector(float x, float y)
         : mX(x),
           mY(y)
      {
      }

      JoystickBallVector()
        : mX(0.0f),
          mY(0.0f)
      {
      }

      int* getXPtr()
      {
         return &mX;
      }

      int* getYPtr()
      {
         return &mY;
      }

   protected:

      int mX;
      int mY;
};

#endif // JOYSTICKBALLVALUE_H
