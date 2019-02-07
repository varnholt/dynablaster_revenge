#ifndef QJOYSTICK_H
#define QJOYSTICK_H

// Qt
#include <QObject>
#include <QString>
#include <QList>

// base
#include "joystickinterface.h"

// SDL
#ifdef SDL1
   #include "../sdl/SDL.h"
#else
#ifdef Q_OS_MAC
#ifdef MAC_OS_X_VERSION_MIN_REQUIRED
#undef MAC_OS_X_VERSION_MIN_REQUIRED
#endif
#define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_9
#endif
   #include "../sdl2/SDL.h"
#endif


class JoystickInterfaceSDL : public JoystickInterface
{
   Q_OBJECT

   public:

      //! constructor
      JoystickInterfaceSDL(QObject* parent = 0);

      //! destructor
      virtual ~JoystickInterfaceSDL();

      // joystick management

      //! get number of joysticks
      virtual int getJoystickCount();

      //! setter for active joystick
      virtual void setActiveJoystick(int id);

      //! getter for active joystick
      virtual int getActiveJoystick();


      // information about the current joystick

      //! getter for the joystick's name
      virtual QString getName(int id);

      //! getter for the axis count
      virtual int getAxisCount(int id);

      //! getter for the button count
      virtual int getButtonCount(int id);

      //! getter for the ball count
      virtual int getBallCount(int id);

      //! getter for the hat count
      virtual int getHatCount(int id);

      //! update axis and button infos
      virtual void update(JoystickInfo& info);


#ifndef SDL1

   public slots:

      //! rumble test
      virtual void rumbleTest();

      //! start rumble effect
      virtual void rumble(float intensity, int ms);


   protected slots:

      //! clean up rumble effect
      void cleanupRumble();


   protected:

      //! bind the dpad buttons
      void bindDpadButtons();

      //! check if button is dpad button
      bool isDpadButton(int button) const;

      //! initialize button mappings
      void initializeButtonMappings();

      //! get button mapping from sdl button mapping
      JoystickConstants::ControllerButton getControllerButton(
         SDL_GameControllerButton button
      ) const;

      //! get button type by button id
      SDL_GameControllerButton getButtonType(int buttonId) const;

#endif

   private:


      //! active joystick
      SDL_Joystick* mActiveJoystick;

      #ifndef SDL1

      SDL_GameController* mController;
      SDL_GameControllerButtonBind mDpadUpBind;
      SDL_GameControllerButtonBind mDpadDownBind;
      SDL_GameControllerButtonBind mDpadLeftBind;
      SDL_GameControllerButtonBind mDpadRightBind;

      //! running haptic effect
      SDL_Haptic* mHaptic;

      #endif
};

#endif // QJOYSTICK_H
