// header
#include "gamejoystickmapping.h"

// game
#include "constants.h"
#include "gamesettings.h"


GameJoystickMapping::GameJoystickMapping(QObject *parent)
   : QObject(parent),
     mKeysPressed(0),
     mKeysPressedPrevious(0),
     mButtonBombId(0),
     mButtonKickId(0),
     mAxisThreshold(3200),
     mHatValue(JoystickConstants::HatCentered),
     mAxisXValue(0),
     mAxisYValue(0),
     mAxisKeys(0),
     mHatKeys(0)
{
   setAxisThreshold(
      GameSettings::getInstance()->getControllerSettings()->getAnalogueThreshold()
   );
}


void GameJoystickMapping::data(const JoystickInfo& info)
{
   mJoystickInfo = info;

   bool hatsChanged = false;
   bool axesChanged = false;

   hatsChanged = processHats();
   axesChanged = processAxes();

   // use hat values
   if (hatsChanged)
   {
      setKeysPressed(getHatKeys());

      emit hat(getHatValue());
   }

   // use axis values
   if (axesChanged)
   {
      setKeysPressed(getAxisKeys());

      int x = 0;
      int y = 0;

      getAxisValues(x, y);

      emit axis(x, y);
   }

   // merge keys pressed with buttons
   processButtons();

   updateKeysPressed();
   setKeysPressedPrevious(getKeysPressed());

   // pass joystick info object to "stakeholders"
   emit dataReceived(info);
}


void GameJoystickMapping::setHatValue(JoystickConstants::Hat hat)
{
   mHatValue = hat;
}


JoystickConstants::Hat GameJoystickMapping::getHatValue() const
{
   return mHatValue;
}


void GameJoystickMapping::setAxisValues(int x, int y)
{
   mAxisXValue = x;
   mAxisYValue = y;
}


void GameJoystickMapping::getAxisValues(int &x, int &y)
{
   x = mAxisXValue;
   y = mAxisYValue;
}


void GameJoystickMapping::setKeysPressed(int keys)
{
   mKeysPressed = keys;
}


int GameJoystickMapping::getKeysPressed() const
{
   return mKeysPressed;
}


void GameJoystickMapping::setKeysPressedPrevious(int keys)
{
   mKeysPressedPrevious = keys;
}


int GameJoystickMapping::getKeysPressedPrevious() const
{
   return mKeysPressedPrevious;
}


void GameJoystickMapping::setAxisKeys(int keys)
{
   mAxisKeys = keys;
}


int GameJoystickMapping::getAxisKeys() const
{
   return mAxisKeys;
}


void GameJoystickMapping::setHatKeys(int keys)
{
   mHatKeys = keys;
}


int GameJoystickMapping::getHatKeys() const
{
   return mHatKeys;
}


void GameJoystickMapping::setAxisThreshold(int threshold)
{
   mAxisThreshold = threshold;
}


int GameJoystickMapping::getAxisThreshold()
{
   return mAxisThreshold;
}


void GameJoystickMapping::updateKeysPressed()
{
   GameSettings::ControllerSettings* controllerSettings =
      GameSettings::getInstance()->getControllerSettings();

   // up
   if (
             (getKeysPressed()         & Constants::KeyUp)
         &&! (getKeysPressedPrevious() & Constants::KeyUp)
   )
   {
      emit keyPressed(controllerSettings->getUpKey());
   }

   if (
             (getKeysPressedPrevious() & Constants::KeyUp)
         &&! (getKeysPressed()         & Constants::KeyUp)
   )
   {
      emit keyReleased(controllerSettings->getUpKey());
   }

   // down
   if (
             (getKeysPressed()         & Constants::KeyDown)
         &&! (getKeysPressedPrevious() & Constants::KeyDown)
   )
   {
      emit keyPressed(controllerSettings->getDownKey());
   }

   if (
             (getKeysPressedPrevious() & Constants::KeyDown)
         &&! (getKeysPressed()         & Constants::KeyDown)
   )
   {
      emit keyReleased(controllerSettings->getDownKey());
   }

   // left
   if (
             (getKeysPressed()         & Constants::KeyLeft)
         &&! (getKeysPressedPrevious() & Constants::KeyLeft)
   )
   {
      emit keyPressed(controllerSettings->getLeftKey());
   }

   if (
             (getKeysPressedPrevious() & Constants::KeyLeft)
         &&! (getKeysPressed()         & Constants::KeyLeft)
   )
   {
      emit keyReleased(controllerSettings->getLeftKey());
   }

   // right
   if (
             (getKeysPressed()         & Constants::KeyRight)
         &&! (getKeysPressedPrevious() & Constants::KeyRight)
   )
   {
      emit keyPressed(controllerSettings->getRightKey());
   }

   if (
             (getKeysPressedPrevious() & Constants::KeyRight)
         &&! (getKeysPressed()         & Constants::KeyRight)
   )
   {
      emit keyReleased(controllerSettings->getRightKey());
   }

   // bomb
   if (
             (getKeysPressed()         & Constants::KeyBomb)
         &&! (getKeysPressedPrevious() & Constants::KeyBomb)
   )
   {
      emit keyPressed(controllerSettings->getBombKey());
   }

   if (
             (getKeysPressedPrevious() & Constants::KeyBomb)
         &&! (getKeysPressed()         & Constants::KeyBomb)
   )
   {
      emit keyReleased(controllerSettings->getBombKey());
   }
}


bool GameJoystickMapping::processHats()
{
   bool changed = false;

   // use hat values
   if (mJoystickInfo.getHatValues().size() > 0)
   {
      setHatValue(mJoystickInfo.getHatValues().at(0));

      int keysModified = getHatKeys();
      int keysPrevious = getHatKeys();

      switch (getHatValue())
      {
         case JoystickConstants::HatCentered:
            keysModified &= ~Constants::KeyUp;
            keysModified &= ~Constants::KeyDown;
            keysModified &= ~Constants::KeyLeft;
            keysModified &= ~Constants::KeyRight;
            break;
         case JoystickConstants::HatUp:
            keysModified |= Constants::KeyUp;
            keysModified &= ~Constants::KeyDown;
            keysModified &= ~Constants::KeyLeft;
            keysModified &= ~Constants::KeyRight;
            break;
         case JoystickConstants::HatRight:
            keysModified |= Constants::KeyRight;
            keysModified &= ~Constants::KeyUp;
            keysModified &= ~Constants::KeyDown;
            keysModified &= ~Constants::KeyLeft;
            break;
         case JoystickConstants::HatDown:
            keysModified |= Constants::KeyDown;
            keysModified &= ~Constants::KeyUp;
            keysModified &= ~Constants::KeyLeft;
            keysModified &= ~Constants::KeyRight;
            break;
         case JoystickConstants::HatLeft:
            keysModified |= Constants::KeyLeft;
            keysModified &= ~Constants::KeyUp;
            keysModified &= ~Constants::KeyDown;
            keysModified &= ~Constants::KeyRight;
            break;
         case JoystickConstants::HatRightUp:
            keysModified |= Constants::KeyRight;
            keysModified |= Constants::KeyUp;
            keysModified &= ~Constants::KeyDown;
            keysModified &= ~Constants::KeyLeft;
            break;
         case JoystickConstants::HatRightDown:
            keysModified |= Constants::KeyRight;
            keysModified |= Constants::KeyDown;
            keysModified &= ~Constants::KeyUp;
            keysModified &= ~Constants::KeyLeft;
            break;
         case JoystickConstants::HatLeftUp:
            keysModified |= Constants::KeyLeft;
            keysModified |= Constants::KeyUp;
            keysModified &= ~Constants::KeyDown;
            keysModified &= ~Constants::KeyRight;
            break;
         case JoystickConstants::HatLeftDown:
            keysModified |= Constants::KeyLeft;
            keysModified |= Constants::KeyDown;
            keysModified &= ~Constants::KeyUp;
            keysModified &= ~Constants::KeyRight;
            break;
      }

      changed = (keysPrevious != keysModified);
      setHatKeys(keysModified);
   }

   return changed;
}


bool GameJoystickMapping::processButtons()
{
   bool changed = false;
   int buttons = 0;

   if (mJoystickInfo.getButtonValues().size() > 0)
   {
      int keysPressed = getKeysPressed();

      // there's only one thing to do by triggering a button
      // => drop a bomb
      bool bombdropped = false;

      int buttonId = 0;
      foreach(bool val, mJoystickInfo.getButtonValues())
      {
         if (val)
         {
            buttons |= 1 << buttonId;

            bombdropped = true;

            // this is not perfect for the controller options part,
            // but the number of shits given is 0
            break;
         }

         buttonId++;
      }

      if (bombdropped)
      {
         setKeysPressed(getKeysPressed() | Constants::KeyBomb);
      }
      else
      {
         setKeysPressed(getKeysPressed() & ~Constants::KeyBomb);
      }

      changed = (keysPressed != getKeysPressed());
   }

   if (changed)
   {
      emit button(buttons);
   }

   return changed;
}


bool GameJoystickMapping::processAxes()
{
   bool changed = false;

   GameSettings::ControllerSettings* controllerSettings =
      GameSettings::getInstance()->getControllerSettings();

   int axis1 = controllerSettings->getAnalogueAxis1();
   int axis2 = controllerSettings->getAnalogueAxis2();

   const QList<int>& axisValues = mJoystickInfo.getAxisValues();

   if (axisValues.size() >= (axis2 + 1))
   {
      int keysModified = getAxisKeys();
      int keysPrevious = getAxisKeys();

      int x = 0;
      int y = 0;

      setAxisValues(
         axisValues.at(axis1),
         axisValues.at(axis2)
      );

      getAxisValues(x, y);

      // x axis
      if (x < -mAxisThreshold)
      {
         // left
         keysModified |= Constants::KeyLeft;
         keysModified &= ~Constants::KeyRight;
      }
      else if (x > mAxisThreshold)
      {
         // right
         keysModified |= Constants::KeyRight;
         keysModified &= ~Constants::KeyLeft;
      }
      else
      {
         // left and right released
         keysModified &= ~Constants::KeyLeft;
         keysModified &= ~Constants::KeyRight;
      }

      // y axis
      if (y < -mAxisThreshold)
      {
         // up
         keysModified |= Constants::KeyUp;
         keysModified &= ~Constants::KeyDown;
      }
      else if (y > mAxisThreshold)
      {
         // down
         keysModified |= Constants::KeyDown;
         keysModified &= ~Constants::KeyUp;
      }
      else
      {
         // up and down released
         keysModified &= ~Constants::KeyUp;
         keysModified &= ~Constants::KeyDown;
      }

      changed = (keysPrevious != keysModified);
      setAxisKeys(keysModified);
   }

   return changed;
}

