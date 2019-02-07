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

      enum ControllerButton
      {
         ControllerButtonInvalid = -1,
          ControllerButtonA,
          ControllerButtonB,
          ControllerButtonX,
          ControllerButtonY,
          ControllerButtonBack,
          ControllerButtonGuide,
          ControllerButtonStart,
          ControllerButtonLeftStick,
          ControllerButtonRightStick,
          ControllerButtonLeftShoulder,
          ControllerButtonRightShoulder,
          ControllerButtonDpadUp,
          ControllerButtonDpadDown,
          ControllerButtonDpadLeft,
          ControllerButtonDpadRight,
          ControllerButtonMax
      };
};


#endif // JOYSTICKCONSTANTS_H
