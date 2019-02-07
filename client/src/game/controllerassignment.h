#ifndef CONTROLLERASSIGNMENT_H
#define CONTROLLERASSIGNMENT_H

class ControllerAssignment
{
   public:

      enum ControllerType
      {
         ControllerTypeJoystick,
         ControllerTypeKeyboard
      };

      ControllerAssignment(ControllerType type);


   protected:

      ControllerType mControllerType;

      //! main controls
      int mUp;
      int mDown;
      int mLeft;
      int mRight;
      int mBomb;

      //! end game
      int mEndGame;

      //! music controls
      int mTrackNext;
      int mTrackPrevious;

      //! zoom controls
      int mZoomIn;
      int mZoomOut;
};

#endif // CONTROLASSIGNMENT_H
