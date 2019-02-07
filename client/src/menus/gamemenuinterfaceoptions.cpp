// header
#include "gamemenuinterfaceoptions.h"

// client
#include "bombermanclient.h"

// menu
#include "gamemenudefines.h"
#include "menu.h"
#include "menupagecheckboxitem.h"
#include "menupagecomboboxitem.h"
#include "menupageitem.h"
#include "menupagelabelitem.h"
#include "menupagetextedit.h"
#include "menupagepixmapitem.h"
#include "menupageslideritem.h"

// settings
#include "gamesettings.h"

// game
#include "gamejoystickintegration.h"
#include "gamejoystickmapping.h"
#include "gamejoystickintegration.h"
#include "joystickcalibration.h"
#include "joystickinterface.h"
#include "keyboardmapper.h"
#include "playerinfo.h"
#include "soundmanager.h"
#include "videooptions.h"

// framework
#include "math/vector.h"

// std
#include "math.h"

// Qt
#include <QKeyEvent>


//-----------------------------------------------------------------------------
/*!
*/
GameMenuInterfaceOptions::GameMenuInterfaceOptions(QObject* parent)
 : GameMenuInterface(parent),
   mButtonA(0),
   mButtonB(0),
   mButtonX(0),
   mButtonY(0),
   mButtonShoulderLeft(0),
   mButtonShoulderRight(0),
   mButtonStart(0),
   mButtonBack(0),
   mKeyMapValid(false),
   mAnalogAxis1Initialized(false),
   mAnalogAxis2Initialized(false),
   mJoystickCalibration(0)
{
   mJoystickCalibration = new JoystickCalibration(this);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::storeOptions()
{
   // pass used audio settings to game settings
   GameSettings::getInstance()->getAudioSettings()->setVolumeMusic(
      SoundManager::getInstance()->getVolumeMusic()
   );

   GameSettings::getInstance()->getAudioSettings()->setVolumeSfx(
      SoundManager::getInstance()->getVolumeSfx()
   );

   // pass controller settings to game settings
   serializeControllerSettings();
   if (isKeyMapValid())
      GameSettings::getInstance()->getControllerSettings()->setKeyMap(mKeyMap);

   // pass video settings to game settings
   serializeVideoSettings();

   // pass gameplay settings to game settings
   serializeGameplaySettings();

   // and store them
   GameSettings::getInstance()->serialize();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::restoreAudioOptions()
{
   // apply volume settings
   applyVolumeMusic(
      GameSettings::getInstance()->getAudioSettings()->getVolumeMusic()
   );

   applyVolumeSfx(
      GameSettings::getInstance()->getAudioSettings()->getVolumeSfx()
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::restoreVideoOptions()
{
   GameSettings::VideoSettings::duplicate(
      GameSettings::getInstance()->getVideoSettings(),
      GameSettings::getInstance()->getVideoSettingsBackup()
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::restoreVideoDefaults()
{
   GameSettings::getInstance()->getVideoSettings()->restoreDefaults();

   // get those values into the menu items
   deserializeVideoSettings();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::restoreGameDefaults()
{
   GameSettings::getInstance()->getGameplaySettings()->restoreDefaults();

   // get those values into the menu items
   deserializeGameplaySettings();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::restoreAudioDefaults()
{
   GameSettings::getInstance()->getAudioSettings()->restoreDefaults();

   // write defaults back to audio manager
   restoreAudioOptions();

   // get those values into the menu items
   deserializeAudioSettings();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::restoreControlsDefaults()
{
   GameSettings::getInstance()->getControllerSettings()->restoreDefaults();
   deserializeControllerSettings();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::serializeGameplaySettings()
{
   // checkbox_cfollow
   // slider_shake

   MenuPage* page = Menu::getInstance()->getPageByName(OPTIONS_GAME);

   MenuPageItem* cameraFollowsPlayer = page->getPageItem("checkbox_cfollow");
   MenuPageItem* shakeIntensity = page->getPageItem("slider_shake");

   MenuPageCheckBoxItem* cameraFollowsPlayerCheckBox =
      dynamic_cast<MenuPageCheckBoxItem*>(cameraFollowsPlayer);

   MenuPageSliderItem* shakeIntensitySlider =
      dynamic_cast<MenuPageSliderItem*>(shakeIntensity);

   float intensity = shakeIntensitySlider->getValue();
   bool follow = cameraFollowsPlayerCheckBox->isChecked();

   GameSettings::GameplaySettings* settings =
      GameSettings::getInstance()->getGameplaySettings();

   settings->setCameraFollowsPlayer(follow);
   settings->setCameraShakeIntensity(intensity);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::deserializeGameplaySettings()
{
   MenuPage* page = Menu::getInstance()->getCurrentPage();

   MenuPageItem* cameraFollowsPlayer = page->getPageItem("checkbox_cfollow");
   MenuPageItem* shakeIntensity = page->getPageItem("slider_shake");

   GameSettings::GameplaySettings* settings =
      GameSettings::getInstance()->getGameplaySettings();

   if (cameraFollowsPlayer)
   {
      MenuPageCheckBoxItem* cameraFollowsPlayerCheckBox =
         dynamic_cast<MenuPageCheckBoxItem*>(cameraFollowsPlayer);

      cameraFollowsPlayerCheckBox->setChecked(
         settings->isCameraFollowingPlayer()
      );
   }

   if (shakeIntensity)
   {
      MenuPageSliderItem* shakeIntensitySlider =
         dynamic_cast<MenuPageSliderItem*>(shakeIntensity);

      shakeIntensitySlider->setValue(
         settings->getCameraShakeIntensity()
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::serializeVideoSettings()
{
   // extract information from ui
   MenuPage* page = Menu::getInstance()->getPageByName(OPTIONS_VIDEO);

   MenuPageItem* resolution = page->getPageItem("combobox_resolution_table");
   MenuPageItem* displayMode = page->getPageItem("combobox_display_table");
   MenuPageItem* antialias = page->getPageItem("combobox_antialiasing_table");
   MenuPageItem* vsync = page->getPageItem("combobox_vsync_table");
   MenuPageItem* fps = page->getPageItem("checkbox_fps");

   MenuPageComboBoxItem* resolutionCombo = dynamic_cast<MenuPageComboBoxItem*>(resolution);
   MenuPageComboBoxItem* displayModeCombo = dynamic_cast<MenuPageComboBoxItem*>(displayMode);
   MenuPageComboBoxItem* antialiasCombo = dynamic_cast<MenuPageComboBoxItem*>(antialias);
   MenuPageComboBoxItem* vsyncCombo = dynamic_cast<MenuPageComboBoxItem*>(vsync);
   MenuPageCheckBoxItem* fpsCheckBox = dynamic_cast<MenuPageCheckBoxItem*>(fps);

   // read mode index from combobox
   int resolutionIndex = resolutionCombo->getActiveElement();

   // read antialias index from combobox
   int displayModeIndex = displayModeCombo->getActiveElement();

   // read fullscreen index from combobox
   int antialiasIndex = antialiasCombo->getActiveElement();

   // read vsync index from combobox
   int vsyncIndex = vsyncCombo->getActiveElement();

   qDebug(
      "GameMenuInterfaceOptions::serializeVideoSettings():\n"
      "resolution: %d\n"
      "display mode: %d\n"
      "antialias: %d\n"
      "vsync: %d\n",
      resolutionIndex,
      displayModeIndex,
      antialiasIndex,
      vsyncIndex
   );

   VideoOptions* options = VideoOptions::getInstance();

   GameSettings::VideoSettings* settings =
      GameSettings::getInstance()->getVideoSettings();

   // set the selected mode's dimensions
   int res= options->getResolution(resolutionIndex);
   settings->setResolution(res);

   // read and set samples from options
   int samples = options->getSamples( antialiasIndex );
   settings->setAntialias(samples);

   // set fullscreen/windowed flag
   settings->setFullscreen(displayModeIndex == 0);
   emit updateFullscreen();

   // check for fps shown
   bool showFps = fpsCheckBox->isChecked();
   settings->setShowFps(showFps);
   emit updateShowFps();

   // set vsync value
   settings->setVSync(vsyncIndex);
   emit updateVsync();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::deserializeVideoSettings()
{
   GameSettings::VideoSettings* settings =
      GameSettings::getInstance()->getVideoSettings();

   VideoOptions* options = VideoOptions::getInstance();
   options->initialize();

   // extract information from ui
   MenuPage* page = Menu::getInstance()->getPageByName(OPTIONS_VIDEO);

   MenuPageItem* resolution = page->getPageItem("combobox_resolution_table");
   MenuPageItem* displayMode = page->getPageItem("combobox_display_table");
   MenuPageItem* antialias = page->getPageItem("combobox_antialiasing_table");
   MenuPageItem* vsync = page->getPageItem("combobox_vsync_table");
   MenuPageItem* brightnessPageItem = page->getPageItem("slider_brightness");

   MenuPageComboBoxItem* resolutionCombo = dynamic_cast<MenuPageComboBoxItem*>(resolution);
   MenuPageComboBoxItem* displayModeCombo = dynamic_cast<MenuPageComboBoxItem*>(displayMode);
   MenuPageComboBoxItem* antialiasCombo = dynamic_cast<MenuPageComboBoxItem*>(antialias);
   MenuPageComboBoxItem* vsyncCombo = dynamic_cast<MenuPageComboBoxItem*>(vsync);

   // init resolutions
   if (resolutionCombo)
   {
      resolutionCombo->clear();

      int index = 0;
      const QList<int>& resolutions= options->getResolutions();

      for (int i=0; i<resolutions.size(); i++)
      {
         int res= resolutions[i];
         if (res == settings->getResolution())
            index= i;
         resolutionCombo->appendItem(
            tr("%1 x %2").arg(res).arg(res)
         );
      }

      QString value = resolutionCombo->getElementText(index);
      resolutionCombo->setValue(value);
      resolutionCombo->setActiveElement(index);
   }

   // init display mode
   if (displayModeCombo)
   {
      // init items
      displayModeCombo->clear();
      displayModeCombo->appendItem(tr("fullscreen"));
      displayModeCombo->appendItem(tr("windowed"));

      // set index
      int index = settings->isFullscreen() ? 0 : 1;
      QString value = displayModeCombo->getElementText(index);
      displayModeCombo->setValue(value);
      displayModeCombo->setActiveElement(index);
   }

   // init antialiasing
   if (antialiasCombo)
   {
      antialiasCombo->clear();

      // look up samples for given mode
      const QList<int>& samples = options->getSampleList();

      int configuredAntialias = settings->getAntialias();
      int selectedIndex = 0;
      int index = 0;

      // add sample values to combobox
      foreach (int value, samples)
      {
         QString option = tr("%1x").arg(value);
         antialiasCombo->appendItem(option);

         // also check which item shall be preselected
         if (value == configuredAntialias)
            selectedIndex = index;

         index++;
      }

      // preselect item
      QString value = antialiasCombo->getElementText(selectedIndex);
      antialiasCombo->setValue(value);
      antialiasCombo->setActiveElement(selectedIndex);
   }


   if (brightnessPageItem)
   {
      MenuPageSliderItem* brightnessSlider =
         dynamic_cast<MenuPageSliderItem*>(brightnessPageItem);

      brightnessSlider->setValue(settings->getBrightness());
   }

   // init display mode
   if (vsyncCombo)
   {
      QList<QString> vals;
      vals << tr("off");
      vals << tr("60fps");
      vals << tr("30fps");

      // init items
      vsyncCombo->clear();

      foreach(QString val, vals)
         vsyncCombo->appendItem(val);

      // set index
      int index = settings->getVSync();

      if (index < 0 || index >= vals.size())
         index = 0;

      QString value = vals.at(index);
      vsyncCombo->setValue(value);
      vsyncCombo->setActiveElement(index);
   }

   GameSettings::VideoSettings::duplicate(
      GameSettings::getInstance()->getVideoSettingsBackup(),
      GameSettings::getInstance()->getVideoSettings()
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::setMonitorVideoSettingsEnabled(bool enabled)
{
   if (enabled)
   {
      MenuPage* currentPage = Menu::getInstance()->getCurrentPage();

      MenuPageItem* brightnessPageItem =
         currentPage->getPageItem("slider_brightness");

      if (brightnessPageItem)
      {
         MenuPageSliderItem* brightnessSlider =
            dynamic_cast<MenuPageSliderItem*>(brightnessPageItem);

         if (brightnessSlider)
         {
            connect(
               brightnessSlider,
               SIGNAL(valueChanged(float)),
               this,
               SLOT(applyBrightness(float))
            );
         }
      }
   }
   else
   {
      disconnect(
         this,
         SLOT(applyBrightness(float))
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::applyBrightness(float value)
{
   GameSettings::getInstance()->getVideoSettings()->setBrightness(value);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::deserializeAudioSettings()
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();

   MenuPageItem* musicVolumeItem = currentPage->getPageItem("slider_music");
   MenuPageItem* sfxVolumeItem = currentPage->getPageItem("slider_game");

   if (musicVolumeItem)
   {
      MenuPageSliderItem* musicVolumeSlider =
         dynamic_cast<MenuPageSliderItem*>(musicVolumeItem);

      musicVolumeSlider->setValue(
         SoundManager::getInstance()->getVolumeMusic()
      );
   }

   if (sfxVolumeItem)
   {
      MenuPageSliderItem* sfxVolumeSlider =
         dynamic_cast<MenuPageSliderItem*>(sfxVolumeItem);

      sfxVolumeSlider->setValue(
         SoundManager::getInstance()->getVolumeSfx()
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::applyVolumeMusic(float volume)
{
   SoundManager::getInstance()->setVolumeMusic(volume);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::applyVolumeSfx(float volume)
{
   SoundManager::getInstance()->setVolumeSfx(volume);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::setMonitorAudioSettingsEnabled(bool enabled)
{
   if (enabled)
   {
      MenuPage* currentPage = Menu::getInstance()->getCurrentPage();

      MenuPageItem* musicVolumeItem = currentPage->getPageItem("slider_music");
      MenuPageItem* sfxVolumeItem = currentPage->getPageItem("slider_game");

      if (musicVolumeItem)
      {
         MenuPageSliderItem* musicVolumeSlider =
            dynamic_cast<MenuPageSliderItem*>(musicVolumeItem);

         if (musicVolumeSlider)
         {
            connect(
               musicVolumeSlider,
               SIGNAL(valueChanged(float)),
               this,
               SLOT(applyVolumeMusic(float))
            );
         }
      }

      if (sfxVolumeItem)
      {
         MenuPageSliderItem* sfxVolumeSlider =
            dynamic_cast<MenuPageSliderItem*>(sfxVolumeItem);

         if (sfxVolumeSlider)
         {
            connect(
               sfxVolumeSlider,
               SIGNAL(valueChanged(float)),
               this,
               SLOT(applyVolumeSfx(float))
            );

            connect(
               sfxVolumeSlider,
               SIGNAL(valueChanged(float)),
               SoundManager::getInstance(),
               SLOT(playSoundTick())
            );
         }
      }
   }
   else
   {
      disconnect(
         this,
         SLOT(applyVolumeMusic(float))
      );

      disconnect(
         this,
         SLOT(applyVolumeSfx(float))
      );

      disconnect(
         SoundManager::getInstance(),
         SLOT(playSoundTick())
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::serializeControllerSettings()
{
   MenuPage* page = Menu::getInstance()->getPageByName(OPTIONS_CONTROLS);

   MenuPageTextEditItem* upKeyboard =
      (MenuPageTextEditItem*)page->getPageItem("lineedit_keyboard_up");

   MenuPageTextEditItem* downKeyboard =
      (MenuPageTextEditItem*)page->getPageItem("lineedit_keyboard_down");

   MenuPageTextEditItem* leftKeyboard =
      (MenuPageTextEditItem*)page->getPageItem("lineedit_keyboard_left");

   MenuPageTextEditItem* rightKeyboard =
      (MenuPageTextEditItem*)page->getPageItem("lineedit_keyboard_right");

   MenuPageTextEditItem* bombKeyboard =
      (MenuPageTextEditItem*)page->getPageItem("lineedit_keyboard_bomb");

   QString upText = upKeyboard->getText();
   QString downText = downKeyboard->getText();
   QString leftText = leftKeyboard->getText();
   QString rightText = rightKeyboard->getText();
   QString bombText = bombKeyboard->getText();

   bool valid = true;

   Qt::Key upKey = KeyboardMapper::getInstance()->getFromString(upText, &valid);
   Qt::Key downKey = KeyboardMapper::getInstance()->getFromString(downText, &valid);
   Qt::Key leftKey = KeyboardMapper::getInstance()->getFromString(leftText, &valid);
   Qt::Key rightKey = KeyboardMapper::getInstance()->getFromString(rightText, &valid);
   Qt::Key bombKey = KeyboardMapper::getInstance()->getFromString(bombText, &valid);

   if (valid)
   {
      if (upKey != Qt::Key_unknown)
      {
          mKeyMap.remove(Constants::KeyUp);
          mKeyMap.insert(Constants::KeyUp, upKey);
      }

      if (downKey != Qt::Key_unknown)
      {
          mKeyMap.remove(Constants::KeyDown);
          mKeyMap.insert(Constants::KeyDown, downKey);
      }

      if (leftKey != Qt::Key_unknown)
      {
          mKeyMap.remove(Constants::KeyLeft);
          mKeyMap.insert(Constants::KeyLeft, leftKey);
      }

      if (rightKey != Qt::Key_unknown)
      {
          mKeyMap.remove(Constants::KeyRight);
          mKeyMap.insert(Constants::KeyRight, rightKey);
      }

      if (bombKey != Qt::Key_unknown)
      {
          mKeyMap.remove(Constants::KeyBomb);
          mKeyMap.insert(Constants::KeyBomb, bombKey);
      }
   }

   setKeyMapValid(valid);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::setKeyMapValid(bool valid)
{
   mKeyMapValid = valid;
}


//-----------------------------------------------------------------------------
/*!
*/
bool GameMenuInterfaceOptions::isKeyMapValid() const
{
   return mKeyMapValid;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::displayJoystickName()
{
   MenuPage* page = Menu::getInstance()->getPageByName(OPTIONS_CONTROLS);

   MenuPageLabelItem* controllerName =
      (MenuPageLabelItem*)page->getPageItem("label_controller_name");

   GameJoystickIntegration* gji = GameJoystickIntegration::getInstance(0);

   if (gji)
   {
      JoystickInterface* ji = gji->getJoystickInterface();

      QString joystickName = ji->getName(ji->getActiveJoystick());
      if (joystickName.length() > 25)
         joystickName = QString("%1...").arg(joystickName.left(25).trimmed());

      controllerName->setText(joystickName);
      controllerName->setCenterWidth(controllerName->getActiveLayer()->getWidth());
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::deserializeControllerSettings()
{
   GameSettings::ControllerSettings* settings =
      GameSettings::getInstance()->getControllerSettings();

   mKeyMap = settings->getKeyMap();

   MenuPage* page = Menu::getInstance()->getPageByName(OPTIONS_CONTROLS);

   MenuPageTextEditItem* upKeyboard =
      (MenuPageTextEditItem*)page->getPageItem("lineedit_keyboard_up");

   MenuPageTextEditItem* downKeyboard =
      (MenuPageTextEditItem*)page->getPageItem("lineedit_keyboard_down");

   MenuPageTextEditItem* leftKeyboard =
      (MenuPageTextEditItem*)page->getPageItem("lineedit_keyboard_left");

   MenuPageTextEditItem* rightKeyboard =
      (MenuPageTextEditItem*)page->getPageItem("lineedit_keyboard_right");

   MenuPageTextEditItem* bombKeyboard =
      (MenuPageTextEditItem*)page->getPageItem("lineedit_keyboard_bomb");

   upKeyboard->setText(
      KeyboardMapper::getInstance()->getKeyString(
         (Qt::Key)mKeyMap[Constants::KeyUp]
      )
   );

   downKeyboard->setText(
      KeyboardMapper::getInstance()->getKeyString(
         (Qt::Key)mKeyMap[Constants::KeyDown]
      )
   );

   leftKeyboard->setText(
      KeyboardMapper::getInstance()->getKeyString(
         (Qt::Key)mKeyMap[Constants::KeyLeft]
      )
   );

   rightKeyboard->setText(
      KeyboardMapper::getInstance()->getKeyString(
         (Qt::Key)mKeyMap[Constants::KeyRight]
      )
   );

   bombKeyboard->setText(
      KeyboardMapper::getInstance()->getKeyString(
         (Qt::Key)mKeyMap[Constants::KeyBomb]
      )
   );

   displayJoystickName();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::processOptionsControlKeyPressed(
   int key,
   const QString &itemName
)
{
   MenuPage* page = Menu::getInstance()->getPageByName(OPTIONS_CONTROLS);

   if (
         itemName == "lineedit_keyboard_up"
      || itemName == "lineedit_keyboard_down"
      || itemName == "lineedit_keyboard_left"
      || itemName == "lineedit_keyboard_right"
      || itemName == "lineedit_keyboard_bomb"
   )
   {
      MenuPageTextEditItem* item =
         (MenuPageTextEditItem*)page->getPageItem(itemName);

      Qt::Key keyType = (Qt::Key)key;

      item->setText(KeyboardMapper::getInstance()->getKeyString(keyType));
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::setMonitorJoystickEnabled(bool enabled)
{
   GameJoystickIntegration* gji = GameJoystickIntegration::getInstance(0);

   if (gji)
   {
      GameJoystickMapping* mapping = gji->getJoystickMapping();

      if (enabled)
      {
         connect(
            mapping,
            SIGNAL(dataReceived(JoystickInfo)),
            this,
            SLOT(processJoystickData(JoystickInfo))
         );

         connect(
            mJoystickCalibration,
            SIGNAL(axesCalibrated(int,int)),
            this,
            SLOT(axesCalibrated(int,int))
         );
      }
      else
      {
         disconnect(
            mapping,
            SIGNAL(dataReceived(JoystickInfo)),
            this,
            SLOT(processJoystickData(JoystickInfo))
         );

         disconnect(
            mJoystickCalibration,
            SIGNAL(axesCalibrated(int,int)),
            this,
            SLOT(axesCalibrated(int,int))
         );
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::resetJoystickStates()
{
   processJoystickHat(JoystickConstants::HatCentered);
   processJoystickAxis(0, 0, 0);
   processJoystickAxis(1, 0, 0);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::initializeButtonLabels()
{
   MenuPage* page = Menu::getInstance()->getPageByName(OPTIONS_CONTROLS);

   // we don't need those
//   MenuPageLabelItem* analogue2Outline =
//      (MenuPageLabelItem*)page->getPageItem(
//         QString("controller_analog_2_outline")
//      );

   MenuPageLabelItem* analogue2Active =
      (MenuPageLabelItem*)page->getPageItem(
         QString("controller_analog_2_active")
      );

//   analogue2Outline->setVisible(false);
   analogue2Active->setVisible(false);

   // store buttons
   mButtonA = (MenuPageLabelItem*)page->getPageItem("label_controller_button_a");
   mButtonB = (MenuPageLabelItem*)page->getPageItem("label_controller_button_b");
   mButtonX = (MenuPageLabelItem*)page->getPageItem("label_controller_button_x");
   mButtonY = (MenuPageLabelItem*)page->getPageItem("label_controller_button_y");

   mButtonShoulderLeft =
      (MenuPageLabelItem*)page->getPageItem("label_controller_button_shoulder_l");

   mButtonShoulderRight =
      (MenuPageLabelItem*)page->getPageItem("label_controller_button_shoulder_r");

   mButtonStart =
      (MenuPageLabelItem*)page->getPageItem("label_controller_button_start");

   mButtonBack =
      (MenuPageLabelItem*)page->getPageItem("label_controller_button_back");

   mAllButtons.clear();
   mAllButtons << mButtonA;
   mAllButtons << mButtonB;
   mAllButtons << mButtonX;
   mAllButtons << mButtonY;
   mAllButtons << mButtonShoulderLeft;
   mAllButtons << mButtonShoulderRight;
   mAllButtons << mButtonStart;
   mAllButtons << mButtonBack;

   // initially hide them all
   foreach (MenuPageLabelItem* item, mAllButtons)
      item->setVisible(false);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::processJoystickHat(JoystickConstants::Hat hatValue)
{
   MenuPage* page = Menu::getInstance()->getPageByName(OPTIONS_CONTROLS);

   MenuPageLabelItem* center =
      (MenuPageLabelItem*)page->getPageItem("label_controller_center");

   MenuPageLabelItem* left =
      (MenuPageLabelItem*)page->getPageItem("label_controller_left");

   MenuPageLabelItem* right =
      (MenuPageLabelItem*)page->getPageItem("label_controller_right");


   MenuPageLabelItem* top =
      (MenuPageLabelItem*)page->getPageItem("label_controller_top");

   MenuPageLabelItem* topLeft =
      (MenuPageLabelItem*)page->getPageItem("label_controller_top_left");

   MenuPageLabelItem* topRight =
      (MenuPageLabelItem*)page->getPageItem("label_controller_top_right");


   MenuPageLabelItem* bottom =
      (MenuPageLabelItem*)page->getPageItem("label_controller_bottom");

   MenuPageLabelItem* bottomLeft =
      (MenuPageLabelItem*)page->getPageItem("label_controller_bottom_left");

   MenuPageLabelItem* bottomRight =
      (MenuPageLabelItem*)page->getPageItem("label_controller_bottom_right");

   QList<MenuPageLabelItem*> items;
   items << center << left << right << top << topLeft << topRight << bottom << bottomLeft << bottomRight;

   MenuPageLabelItem* item = 0;
   switch (hatValue)
   {
      case JoystickConstants::HatUp:
         item = top;
         break;
      case JoystickConstants::HatRightUp:
         item = topRight;
         break;
      case JoystickConstants::HatLeftUp:
         item = topLeft;
         break;
      case JoystickConstants::HatDown:
         item = bottom;
         break;
      case JoystickConstants::HatRightDown:
         item = bottomRight;
         break;
      case JoystickConstants::HatLeftDown:
         item = bottomLeft;
         break;
      case JoystickConstants::HatLeft:
         item = left;
         break;
      case JoystickConstants::HatRight:
         item = right;
         break;
      default:
      case JoystickConstants::HatCentered:
         item = center;
         break;
   }

   items.removeOne(item);

   foreach(MenuPageLabelItem* item, items)
   {
      item->setVisible(false);
   }

   if (item)
   {
      item->setVisible(true);
      item->setAlpha(255);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param button id of pressed button
*/
void GameMenuInterfaceOptions::processJoystickButton(int button, bool enabled)
{
   GameJoystickIntegration* jItegration = GameJoystickIntegration::getInstance(0);

   JoystickInterface* jInterface = jItegration->getJoystickInterface();

   JoystickConstants::ControllerButton* mapping =
      jInterface->getButtonMapping();

   bool enableAll = false;

   if (mapping)
   {
      int count = jInterface->getButtonCount(0);

      if (button < count)
      {
         JoystickConstants::ControllerButton type = mapping[button];
         MenuPageLabelItem* item = getButton(type);

         if (item)
         {
            if (enabled)
               item->setVisible(enabled);
         }
         else
         {
            // just highlight all
            if (enabled)
            {
               enableAll = true;
            }
         }
      }
   }
   else
   {
      if (enabled)
      {
         enableAll = true;
      }
   }

   if (enableAll)
   {
      foreach (MenuPageLabelItem* item, mAllButtons)
         item->setVisible(true);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param buttonType button enum type
   \return button
*/
MenuPageLabelItem* GameMenuInterfaceOptions::getButton(
   JoystickConstants::ControllerButton buttonType
) const
{
   MenuPageLabelItem* buttonLabel = 0;

   switch (buttonType)
   {
      case JoystickConstants::ControllerButtonA:
         buttonLabel = mButtonA;
         break;
      case JoystickConstants::ControllerButtonB:
         buttonLabel = mButtonB;
         break;
      case JoystickConstants::ControllerButtonX:
         buttonLabel = mButtonX;
         break;
      case JoystickConstants::ControllerButtonY:
         buttonLabel = mButtonY;
         break;
      case JoystickConstants::ControllerButtonBack:
         buttonLabel = mButtonBack;
         break;
      case JoystickConstants::ControllerButtonGuide:
         break;
      case JoystickConstants::ControllerButtonStart:
         buttonLabel = mButtonStart;
         break;
      case JoystickConstants::ControllerButtonLeftStick:
         break;
      case JoystickConstants::ControllerButtonRightStick:
         break;
      case JoystickConstants::ControllerButtonLeftShoulder:
         buttonLabel = mButtonShoulderLeft;
         break;
      case JoystickConstants::ControllerButtonRightShoulder:
         buttonLabel = mButtonShoulderRight;
         break;
      case JoystickConstants::ControllerButtonDpadUp:
         break;
      case JoystickConstants::ControllerButtonDpadDown:
         break;
      case JoystickConstants::ControllerButtonDpadLeft:
         break;
      case JoystickConstants::ControllerButtonDpadRight:
         break;
      default:
         break;
   }

   return buttonLabel;
}


//-----------------------------------------------------------------------------
/*!
   \param axisGroup axis group
   \param xInt x value
   \param yInt y value
*/
void GameMenuInterfaceOptions::processJoystickAxis(int axisGroup, int xInt, int yInt)
{
   float x = xInt / 32767.0f;
   float y = yInt / 32767.0f;

   MenuPage* page = Menu::getInstance()->getPageByName(OPTIONS_CONTROLS);

   MenuPageLabelItem* outline =
      (MenuPageLabelItem*)page->getPageItem(
         QString("controller_analog_%1_outline").arg(axisGroup+1)
      );

   MenuPageLabelItem* active =
      (MenuPageLabelItem*)page->getPageItem(
         QString("controller_analog_%1_active").arg(axisGroup+1)
      );

   if (axisGroup == 0)
   {
      if (!mAnalogAxis1Initialized)
      {
         mAnalogAxis1ActivePos.setX(active->getCurrentLayer()->getLeft());
         mAnalogAxis1ActivePos.setY(active->getCurrentLayer()->getTop());
         mAnalogAxis1OutlinePos.setX(outline->getCurrentLayer()->getLeft());
         mAnalogAxis1OutlinePos.setY(outline->getCurrentLayer()->getTop());

         mAnalogAxis1Initialized = true;
      }
   }

   else if (axisGroup == 1)
   {
      if (!mAnalogAxis2Initialized)
      {
         mAnalogAxis2ActivePos.setX(active->getCurrentLayer()->getLeft());
         mAnalogAxis2ActivePos.setY(active->getCurrentLayer()->getTop());
         mAnalogAxis2OutlinePos.setX(outline->getCurrentLayer()->getLeft());
         mAnalogAxis2OutlinePos.setY(outline->getCurrentLayer()->getTop());

         mAnalogAxis2Initialized = true;
      }
   }

   float factor = 10.0f;
   float dx = factor * x;
   float dy = factor * y;

   if (axisGroup == 0)
   {
      active->getCurrentLayer()->setX(mAnalogAxis1ActivePos.x());
      active->getCurrentLayer()->setY(mAnalogAxis1ActivePos.y());
      outline->getCurrentLayer()->setX(mAnalogAxis1OutlinePos.x());
      outline->getCurrentLayer()->setY(mAnalogAxis1OutlinePos.y());
   }

   else if (axisGroup == 1)
   {
      active->getCurrentLayer()->setX(mAnalogAxis2ActivePos.x());
      active->getCurrentLayer()->setY(mAnalogAxis2ActivePos.y());
      outline->getCurrentLayer()->setX(mAnalogAxis2OutlinePos.x());
      outline->getCurrentLayer()->setY(mAnalogAxis2OutlinePos.y());
   }

   active->getCurrentLayer()->move(dx, dy);
   outline->getCurrentLayer()->move(dx, dy);
}


//-----------------------------------------------------------------------------
/*!
   \param joystickInfo joystickInfo object
*/
void GameMenuInterfaceOptions::processJoystickData(const JoystickInfo & joystickInfo)
{
   // update hats
   if (joystickInfo.getHatValues().size() > 0)
   {
      processJoystickHat(joystickInfo.getHatValues().at(0));
   }

   // update buttons
   foreach (MenuPageLabelItem* item, mAllButtons)
      item->setVisible(false);


   if (joystickInfo.getButtonValues().size() > 0)
   {
      int number = 0;
      foreach(bool val, joystickInfo.getButtonValues())
      {
         processJoystickButton(number, val);
         number++;
      }
   }

   // update axes

   /*
   for (int i = 0; i < 2; i++)
   {
      if (joystickInfo.getAxisValues().size() >= ((i + 1) * 2) )
      {
         processJoystickAxis(
            i,
            joystickInfo.getAxisValues().at( (i * 2)    ),
            joystickInfo.getAxisValues().at( (i * 2) + 1)
         );
      }
   }
   */

   GameSettings::ControllerSettings* controllerSettings =
      GameSettings::getInstance()->getControllerSettings();

   const QList<int>& axisValues = joystickInfo.getAxisValues();

   if (axisValues.size() > controllerSettings->getAnalogueAxis2())
   {
      processJoystickAxis(
         0,
         axisValues.at(controllerSettings->getAnalogueAxis1()),
         axisValues.at(controllerSettings->getAnalogueAxis2())
      );

      mJoystickCalibration->processJoystickData(joystickInfo);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceOptions::axesCalibrated(int axis1, int axis2)
{
   GameSettings::getInstance()->getControllerSettings()->setAnalogueAxis1(axis1);
   GameSettings::getInstance()->getControllerSettings()->setAnalogueAxis2(axis2);
}

