// header
#include "joystickinterfacesdl.h"

// Qt
#include <QTimer>


//-----------------------------------------------------------------------------
/*!
   \param parent parent widget
*/
JoystickInterfaceSDL::JoystickInterfaceSDL(QObject* parent)
   : JoystickInterface(parent),
     mActiveJoystick(0)
   #ifndef SDL1
     ,mController(0)
     ,mHaptic(0)
   #endif
{
   #ifdef SDL1
      SDL_Init(SDL_INIT_JOYSTICK);
   #else
      SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC);
      SDL_GameControllerAddMappingsFromFile("data/game/gamecontrollerdb.txt");
   #endif

   // QTimer::singleShot(10000, this, SLOT(rumbleTest()));
}


//-----------------------------------------------------------------------------
/*!
*/
JoystickInterfaceSDL::~JoystickInterfaceSDL()
{
   #ifdef SDL1
   SDL_JoystickClose(mActiveJoystick);
   SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
   #else
   SDL_GameControllerClose(mController);
   SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC);
   delete getButtonMapping();
   #endif
}


//-----------------------------------------------------------------------------
/*!
  \param id joystick id
  \return joystick name1
*/
QString JoystickInterfaceSDL::getName(int id)
{
   QString name;

   if (
         id < getJoystickCount()
      && id >= 0
   )
   {
      #ifdef SDL1
         name = SDL_JoystickName(id);
      #else
         name = SDL_JoystickNameForIndex(id);
      #endif
   }

   return name;
}


//-----------------------------------------------------------------------------
/*!
   \return axis count for joystick with given axis
*/
int JoystickInterfaceSDL::getAxisCount(int id)
{
   int count = 0;

   if (
         id < getJoystickCount()
      && id >= 0
   )
   {
      count = SDL_JoystickNumAxes(mActiveJoystick);
   }

   return count;
}


//-----------------------------------------------------------------------------
/*!
   \return button count for joystick with given id
*/
int JoystickInterfaceSDL::getButtonCount(int id)
{
   int count = 0;

   if (
         id < getJoystickCount()
      && id >= 0
   )
   {
      count = SDL_JoystickNumButtons(mActiveJoystick);
   }

   return count;
}


//-----------------------------------------------------------------------------
/*!
   \return ball count for joystick with given id
*/
int JoystickInterfaceSDL::getBallCount(int id)
{
   int count = 0;

   if (
         id < getJoystickCount()
      && id >= 0
   )
   {
      count = SDL_JoystickNumBalls(mActiveJoystick);
   }

   return count;
}

//-----------------------------------------------------------------------------
/*!
   \return hat count for joystick with given id
*/
int JoystickInterfaceSDL::getHatCount(int id)
{
   int count = 0;

   if (
         id < getJoystickCount()
      && id >= 0
   )
   {
      count = SDL_JoystickNumHats(mActiveJoystick);
   }

   return count;
}


//-----------------------------------------------------------------------------
/*!
   \param id of active joystick
*/
void JoystickInterfaceSDL::setActiveJoystick(int id)
{
   if (
         id < getJoystickCount()
      && id >= 0
   )
   {
      #ifdef SDL1

      SDL_JoystickClose(mActiveJoystick);
      mActiveJoystick = SDL_JoystickOpen(id);

      #else

      if (SDL_IsGameController(id))
      {
         // store controller data
         mController = SDL_GameControllerOpen(id);
         mActiveJoystick = SDL_GameControllerGetJoystick(mController);

         initializeButtonMappings();

         // create dpad bindings
         bindDpadButtons();
      }
      else
      {
         mActiveJoystick = SDL_JoystickOpen(id);
      }

      #endif
   }
}


//-----------------------------------------------------------------------------
/*!
   \return id of active joystick
*/
int JoystickInterfaceSDL::getActiveJoystick()
{
   #ifdef SDL1
      return SDL_JoystickIndex(mActiveJoystick);
   #else
      return SDL_JoystickInstanceID(mActiveJoystick);
   #endif
}


//-----------------------------------------------------------------------------
/*!
   \return number of joysticks
*/
int JoystickInterfaceSDL::getJoystickCount()
{
   return SDL_NumJoysticks();
}


//-----------------------------------------------------------------------------
/*!
  \param joystick info object
*/
void JoystickInterfaceSDL::update(JoystickInfo& info)
{
   info.clear();

   SDL_JoystickUpdate();

   // read axis values
   for (int i = 0; i < SDL_JoystickNumAxes(mActiveJoystick); i++)
      info.addAxisValue(SDL_JoystickGetAxis(mActiveJoystick, i));

   // read button values
   for (int i = 0; i < SDL_JoystickNumButtons(mActiveJoystick); i++)
   {
      #ifndef SDL1

      // do not place bombs on dpad pressed, but also do not screw up the
      // button id order
      if (isDpadButton(i))
         info.addButtonState(false);
      else
      #endif
         info.addButtonState(SDL_JoystickGetButton(mActiveJoystick, i));
   }

   /*
   // read ball values
   for (int i = 0; i < SDL_JoystickNumBalls(mActiveJoystick); i++)
   {
      JoystickBallVector bv;

      SDL_JoystickGetBall(
         mActiveJoystick,
         i,
         bv.getXPtr(),
         bv.getYPtr()
      );

      info.addBallValue(bv);
   }
   */

   int hatCount = SDL_JoystickNumHats(mActiveJoystick);

   #ifndef SDL1
   // emulate hat by evaluating the dpad buttons. some drivers do not register
   // the controller's dpad as hat so they just show up as ordinary buttons.
   // we don't want that.
   if (hatCount == 0)
   {
      JoystickConstants::Hat hat = JoystickConstants::HatCentered;

      bool up    = SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_DPAD_UP);
      bool down  = SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
      bool left  = SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
      bool right = SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

      if (left && up)
         hat = JoystickConstants::HatLeftUp;
      else if (left && down)
         hat = JoystickConstants::HatLeftDown;
      else if (right && up)
         hat = JoystickConstants::HatRightUp;
      else if (right && down)
         hat = JoystickConstants::HatRightDown;
      else if (up)
         hat = JoystickConstants::HatUp;
      else if (down)
         hat = JoystickConstants::HatDown;
      else if (left)
         hat = JoystickConstants::HatLeft;
      else if (right)
         hat = JoystickConstants::HatRight;

      info.addHatValue(hat);
   }
   #endif

   // read hat values
   for (int i = 0; i < hatCount; i++)
   {
      JoystickConstants::Hat hat = JoystickConstants::HatCentered;

      switch (SDL_JoystickGetHat(mActiveJoystick, i))
      {
         case SDL_HAT_CENTERED:
            hat = JoystickConstants::HatCentered;
            break;

         case SDL_HAT_UP:
            hat = JoystickConstants::HatUp;
            break;

         case SDL_HAT_RIGHT:
            hat = JoystickConstants::HatRight;
            break;

         case SDL_HAT_DOWN:
            hat = JoystickConstants::HatDown;
            break;

         case SDL_HAT_LEFT:
            hat = JoystickConstants::HatLeft;
            break;

         case SDL_HAT_RIGHTUP:
            hat = JoystickConstants::HatRightUp;
            break;

         case SDL_HAT_RIGHTDOWN:
            hat = JoystickConstants::HatRightDown;
            break;

         case SDL_HAT_LEFTUP:
            hat = JoystickConstants::HatLeftUp;
            break;

         case SDL_HAT_LEFTDOWN:
            hat = JoystickConstants::HatLeftDown;
            break;

         default:
            break;
      }

      info.addHatValue(hat);
   }
}



#ifndef SDL1


//-----------------------------------------------------------------------------
/*!
*/
void JoystickInterfaceSDL::rumbleTest()
{
   rumble(1.0, 2000);
}


//-----------------------------------------------------------------------------
/*!
   \param intensity rumble intensity
   \param ms rumble time
*/
void JoystickInterfaceSDL::rumble(float intensity, int ms)
{
   if (!mHaptic)
   {
      if (mActiveJoystick)
      {
         // open the device
         mHaptic = SDL_HapticOpenFromJoystick(mActiveJoystick);

         if (mHaptic)
         {
            // initialize simple rumble
            if (SDL_HapticRumbleInit(mHaptic) == 0)
            {
               if (SDL_HapticRumblePlay(mHaptic, intensity, ms) == 0)
               {
                  QTimer::singleShot(
                     ms,
                     this,
                     SLOT(cleanupRumble())
                  );
               }
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void JoystickInterfaceSDL::cleanupRumble()
{
   if (mHaptic)
      SDL_HapticClose(mHaptic);

   mHaptic = 0;
}



//-----------------------------------------------------------------------------
/*!
*/
void JoystickInterfaceSDL::bindDpadButtons()
{
   mDpadUpBind =
      SDL_GameControllerGetBindForButton(
         mController,
         SDL_CONTROLLER_BUTTON_DPAD_UP
      );

   mDpadDownBind =
      SDL_GameControllerGetBindForButton(
         mController,
         SDL_CONTROLLER_BUTTON_DPAD_DOWN
      );

   mDpadLeftBind =
      SDL_GameControllerGetBindForButton(
         mController,
         SDL_CONTROLLER_BUTTON_DPAD_LEFT
      );

   mDpadRightBind =
      SDL_GameControllerGetBindForButton(
         mController,
         SDL_CONTROLLER_BUTTON_DPAD_RIGHT
      );

   if (mDpadUpBind.bindType == SDL_CONTROLLER_BINDTYPE_NONE)
   {
      mDpadUpBind.value.axis = -1;
      mDpadUpBind.value.button = -1;
      mDpadUpBind.value.hat.hat = -1;
      mDpadUpBind.value.hat.hat_mask = -1;
   }

   if (mDpadDownBind.bindType == SDL_CONTROLLER_BINDTYPE_NONE)
   {
      mDpadDownBind.value.axis = -1;
      mDpadDownBind.value.button = -1;
      mDpadDownBind.value.hat.hat = -1;
      mDpadDownBind.value.hat.hat_mask = -1;
   }

   if (mDpadLeftBind.bindType == SDL_CONTROLLER_BINDTYPE_NONE)
   {
      mDpadLeftBind.value.axis = -1;
      mDpadLeftBind.value.button = -1;
      mDpadLeftBind.value.hat.hat = -1;
      mDpadLeftBind.value.hat.hat_mask = -1;
   }

   if (mDpadRightBind.bindType == SDL_CONTROLLER_BINDTYPE_NONE)
   {
      mDpadRightBind.value.axis = -1;
      mDpadRightBind.value.button = -1;
      mDpadRightBind.value.hat.hat = -1;
      mDpadRightBind.value.hat.hat_mask = -1;
   }
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if button is a dpad button
*/
bool JoystickInterfaceSDL::isDpadButton(int button) const
{
   bool dPadButton = false;

   dPadButton = (
          (button == mDpadUpBind.value.button    && mDpadUpBind.bindType    == SDL_CONTROLLER_BINDTYPE_BUTTON)
       || (button == mDpadDownBind.value.button  && mDpadDownBind.bindType  == SDL_CONTROLLER_BINDTYPE_BUTTON)
       || (button == mDpadLeftBind.value.button  && mDpadLeftBind.bindType  == SDL_CONTROLLER_BINDTYPE_BUTTON)
       || (button == mDpadRightBind.value.button && mDpadRightBind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON)
   );

   return dPadButton;
}


//-----------------------------------------------------------------------------
/*!
*/
void JoystickInterfaceSDL::initializeButtonMappings()
{
   int buttonCount = getButtonCount(0);

   // clear previous mapping
   delete getButtonMapping();

   JoystickConstants::ControllerButton* mapping =
      new JoystickConstants::ControllerButton[buttonCount];

   // convert button type from sdl to sdl independent
   SDL_GameControllerButton sdlType =
      SDL_CONTROLLER_BUTTON_INVALID;

   JoystickConstants::ControllerButton buttonType =
      JoystickConstants::ControllerButtonInvalid;

   for (int i = 0; i < buttonCount; i++)
   {
      sdlType = getButtonType(i);
      buttonType = getControllerButton(sdlType);
      mapping[i] = buttonType;
   }

   setButtonMapping(mapping);
}


//-----------------------------------------------------------------------------
/*!
   \param buttonId button id
   \return controller button
*/
SDL_GameControllerButton JoystickInterfaceSDL::getButtonType(int buttonId) const
{
   SDL_GameControllerButton buttonType = SDL_CONTROLLER_BUTTON_INVALID;
   SDL_GameControllerButton tmpType = SDL_CONTROLLER_BUTTON_INVALID;

   SDL_GameControllerButtonBind binding;

   for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
   {
      tmpType = (SDL_GameControllerButton)i;

      binding = SDL_GameControllerGetBindForButton(
         mController,
         tmpType
      );

      if (binding.value.button == buttonId)
      {
         buttonType = tmpType;
         break;
      }
   }

   return buttonType;
}


//-----------------------------------------------------------------------------
/*!
   \param button sdl button
   \return button controller type button
*/
JoystickConstants::ControllerButton JoystickInterfaceSDL::getControllerButton(
   SDL_GameControllerButton button
) const
{
   JoystickConstants::ControllerButton cb =
      JoystickConstants::ControllerButtonInvalid;

   switch (button)
   {
      case SDL_CONTROLLER_BUTTON_A:
         cb = JoystickConstants::ControllerButtonA;
         break;
      case SDL_CONTROLLER_BUTTON_B:
         cb = JoystickConstants::ControllerButtonB;
         break;
      case SDL_CONTROLLER_BUTTON_X:
         cb = JoystickConstants::ControllerButtonX;
         break;
      case SDL_CONTROLLER_BUTTON_Y:
         cb = JoystickConstants::ControllerButtonY;
         break;
      case SDL_CONTROLLER_BUTTON_BACK:
         cb = JoystickConstants::ControllerButtonBack;
         break;
      case SDL_CONTROLLER_BUTTON_GUIDE:
         cb = JoystickConstants::ControllerButtonGuide;
         break;
      case SDL_CONTROLLER_BUTTON_START:
         cb = JoystickConstants::ControllerButtonStart;
         break;
      case SDL_CONTROLLER_BUTTON_LEFTSTICK:
         cb = JoystickConstants::ControllerButtonLeftStick;
         break;
      case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
         cb = JoystickConstants::ControllerButtonRightStick;
         break;
      case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
         cb = JoystickConstants::ControllerButtonLeftShoulder;
         break;
      case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
         cb = JoystickConstants::ControllerButtonRightShoulder;
         break;
      case SDL_CONTROLLER_BUTTON_DPAD_UP:
         cb = JoystickConstants::ControllerButtonDpadUp;
         break;
      case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
         cb = JoystickConstants::ControllerButtonDpadDown;
         break;
      case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
         cb = JoystickConstants::ControllerButtonDpadLeft;
         break;
      case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
         cb = JoystickConstants::ControllerButtonDpadRight;
         break;
      default:
         break;
   }

   return cb;
}

#endif
