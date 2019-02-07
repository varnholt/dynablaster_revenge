#ifndef GAMEMENUINTERFACEOPTIONS_H
#define GAMEMENUINTERFACEOPTIONS_H

// base
#include "gamemenuinterface.h"

// game
#include "constants.h"

// joystick
#include "joystick/joystickconstants.h"
#include "joystick/joystickinfo.h"

// video options
#include "videooptionssdl.h"

// Qt
#include <QPoint>

// forward declarations
class JoystickCalibration;
class MenuPageLabelItem;

class GameMenuInterfaceOptions : public GameMenuInterface
{
   Q_OBJECT

public:

   //! constructor
   GameMenuInterfaceOptions(QObject* parent = 0);

   // options

   //! store current options
   void storeOptions();

   //! restore audio options
   void restoreAudioOptions();

   //! restore video options
   void restoreVideoOptions();

   //! restore video defaults
   void restoreVideoDefaults();

   //! restore game defaults
   void restoreGameDefaults();

   //! restore audio defaults
   void restoreAudioDefaults();

   //! restore controls defaults
   void restoreControlsDefaults();


   // game options

   //! serialize game settings
   void serializeGameplaySettings();

   //! deserialize game settings
   void deserializeGameplaySettings();


   // video options

   //! serialize video settings
   void serializeVideoSettings();

   //! deserialize video settings
   void deserializeVideoSettings();

   //! enabled or disable video setting monitor
   void setMonitorVideoSettingsEnabled(bool enabled);


   // audio options

   //! serialize audio settings
   void serializeAudioSettings();

   //! deserialize audio settings
   void deserializeAudioSettings();

   //! enabled or disable audio setting monitor
   void setMonitorAudioSettingsEnabled(bool enabled);


   // controller options

   //! serialize controller settings
   void serializeControllerSettings();

   //! deserialize controller settings
   void deserializeControllerSettings();

   //! process keyboard press in control options
   void processOptionsControlKeyPressed(
      int key,
      const QString& itemName
   );

   //! enabled or disable joystick monitor
   void setMonitorJoystickEnabled(bool enabled);

   //! reset joystick states
   void resetJoystickStates();

   //! initialize buttons
   void initializeButtonLabels();


signals:

   //! update fullscreen mode
   void updateFullscreen();

   //! update fps shown
   void updateShowFps();

   //! update vsync flag
   void updateVsync();


private slots:

   // audio options

   //! apply music volume
   void applyVolumeMusic(float);

   //! apply sfx volume
   void applyVolumeSfx(float);


   // video options

   //! apply brightness
   void applyBrightness(float);


   // controller options

   //! hat was pressed
   void processJoystickHat(JoystickConstants::Hat hatValue);

   //! button was pressed
   void processJoystickButton(int button, bool enabled);

   //! axis was triggered
   void processJoystickAxis(int axisGroup, int x, int y);

   //! process joystick data
   void processJoystickData(const JoystickInfo&);

   //! axes have been calibrated
   void axesCalibrated(int axis1, int axis2);

   //! display the controller's name
   void displayJoystickName();


private:

   //! check if key map is valid
   bool isKeyMapValid() const;

   //! setter for key map valid flag
   void setKeyMapValid(bool valid);

   //! get button item by button identifier
   MenuPageLabelItem* getButton(JoystickConstants::ControllerButton button) const;


   MenuPageLabelItem* mButtonA;
   MenuPageLabelItem* mButtonB;
   MenuPageLabelItem* mButtonX;
   MenuPageLabelItem* mButtonY;
   MenuPageLabelItem* mButtonShoulderLeft;
   MenuPageLabelItem* mButtonShoulderRight;
   MenuPageLabelItem* mButtonStart;
   MenuPageLabelItem* mButtonBack;
   QList<MenuPageLabelItem*> mAllButtons;

   // controller settings

   //! key map
   QMap<Constants::Key, int> mKeyMap;
   bool mKeyMapValid;

   bool mAnalogAxis1Initialized;
   bool mAnalogAxis2Initialized;
   QPointF mAnalogAxis1ActivePos;
   QPointF mAnalogAxis1OutlinePos;
   QPointF mAnalogAxis2ActivePos;
   QPointF mAnalogAxis2OutlinePos;

   JoystickCalibration* mJoystickCalibration;
};

#endif // GAMEMENUINTERFACEOPTIONS_H
