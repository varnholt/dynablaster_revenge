#ifndef JOYSTICKCONSTANTS_H
#define JOYSTICKCONSTANTS_H

class JoystickConstants
{
   public:

      enum Hat
      {
         HatCentered  = 0,
         HatUp        = 1,
         HatRight     = 2,
         HatDown      = 3,
         HatLeft      = 4,
         HatRightUp   = 5,
         HatRightDown = 6,
         HatLeftUp    = 7,
         HatLeftDown  = 8
      };

      // a= 10
      // b= 11
      // x= 12
      // y= 13
      // left shoulder= 8
      // right shoulder= 9
      // start= 15
      // select= 5
      // left stick= 6
      // right stick= 7

      enum ControllerButton
      {
         ControllerButtonInvalid = -1,
         ControllerButtonDpadUp = 0,
         ControllerButtonDpadDown = 1,
         ControllerButtonDpadLeft = 2,
         ControllerButtonDpadRight = 3,
         ControllerButtonStart = 4,
         ControllerButtonBack = 5,
         ControllerButtonLeftStick = 6,
         ControllerButtonRightStick = 7,
         ControllerButtonLeftShoulder = 8,
         ControllerButtonRightShoulder = 9,
         ControllerButtonA = 10,
         ControllerButtonB = 11,
         ControllerButtonX = 12,
         ControllerButtonY = 13,
         ControllerButtonGuide = 14, // ?! home
         ControllerButtonMax
      };
};


#endif // JOYSTICKCONSTANTS_H
