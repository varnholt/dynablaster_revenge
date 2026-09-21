#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

// Qt
#include <QColor>
#include <QSettings>
#include <QObject>

// shared
#include "constants.h"

class GameSettings
{
   public:

      //! settings base
      class SettingsPrivate : public QSettings
      {
         public:

            //! constructor
            SettingsPrivate();

            //! destructor
            virtual ~SettingsPrivate();

            //! deserialize data
            virtual void deserialize();

            //! serialize data
            virtual void serialize();

            //! restore defaults
            virtual void restoreDefaults();
      };

      //! design settings
      class StyleSettings : public SettingsPrivate
      {

         public:

            //! constructor
            StyleSettings();

            //! deserialize data
            void deserialize();

            //! serialize map
            void serialize();

            //! getter for color
            QColor getColor(Constants::Color color) const;

            //! getter for rgb
            QRgb getRgb(Constants::Color color) const;


         protected:

            //! init default map
            void initDefaultMap();

            //! init individual colors
            void initializeIndividualColor();

            //! color map
            QMap<Constants::Color, QColor> mPlayerColors;

            QRgb mColorWhite;
            QRgb mColorBlack;
            QRgb mColorRed;
            QRgb mColorGreen;
            QRgb mColorBlue;
            QRgb mColorGrey;
            QRgb mColorYellow;
            QRgb mColorPurple;
            QRgb mColorCyan;
            QRgb mColorOrange;
      };


      //! development settings
      class DevelopmentSettings : public SettingsPrivate
      {
         public:

            //! constructor
            DevelopmentSettings();

            //! deserialize data
            void deserialize();

            //! getter for skip menu flag
            bool isSkipMenuEnabled();

            //! getter for splash screen flag
            bool isSplashScreenEnabled();

            //! getter for music enabled flag
            bool isMusicEnabled();

            //! getter for development level
            const QString& getLevel() const;

            //! getter for dryrun flag
            bool isDryRunEnabled() const;

            //! setter for dryrun flag
            void setDryRunEnabled(bool enabled);

            //! getter for game recording flag
            bool isGameRecordingEnabled() const;

            //! setter for game recording flag
            void setGameRecordingEnabled(bool value);

            //! getter for joysticks enabled flag
            bool isJoysticksEnabled() const;

            //! setter for joysticks enabled flag
            void setJoysticksEnabled(bool value);

            //! getter for pouet page
            QString getPagePouet() const;

            //! setter for pouet page
            void setPagePouet(const QString &value);

            //! getter for facebook page
            QString getPageFacebook() const;

            //! setter for facebook page
            void setPageFacebook(const QString &value);

            //! getter for webpage
            QString getPageHome() const;

            //! setter for webpage
            void setPageHome(const QString &value);


      protected:

            //! skip menu
            bool mSkipMenu;

            //! show splash screen
            bool mShowSplash;

            //! music enabled flag
            bool mMusicEnabled;

            //! preselected level
            QString mLevel;

            //! dryrun
            bool mDryRunEnabled;

            //! game recording enabled
            bool mGameRecordingEnabled;

            //! joysticks enabled
            bool mJoysticksEnabled;

            //! pouet page
            QString mPagePouet;

            //! facebook page
            QString mPageFacebook;

            //! webpage
            QString mPageHome;
      };


      //! game settings
      class GameplaySettings : public SettingsPrivate
      {
         public:

            //! constructor
            GameplaySettings();

            //! serialize audio settings
            void serialize();

            //! deserialize audio settings
            void deserialize();

            //! restore audio settings
            void restoreDefaults();

            //! getter for shake intensity
            float getCameraShakeIntensity() const;

            //! setter for shake intensity
            void setCameraShakeIntensity(float value);

            //! getter for camera follows player flag
            bool isCameraFollowingPlayer() const;

            //! setter for camera follows player flag
            void setCameraFollowsPlayer(bool value);


      protected:

            //! camera shake intensity
            float mCameraShakeIntensity;

            //! camera follows player
            bool mCameraFollowsPlayer;
      };


      //! audio settings
      class AudioSettings : public SettingsPrivate
      {
         public:

            //! constructor
            AudioSettings();

            //! serialize audio settings
            void serialize();

            //! deserialize audio settings
            void deserialize();

            //! restore audio settings
            void restoreDefaults();

            //! getter for music volume
            float getVolumeMusic() const;

            //! getter for sfx volume
            float getVolumeSfx() const;

            //! setter for music volume
            void setVolumeMusic(float);

            //! setter for sfx volume
            void setVolumeSfx(float);

            //! getter for default music volume
            float getVolumeMusicDefault() const;

            //! getter for default sfx volume
            float getVolumeSfxDefault() const;

            //! setter for default music volume
            void setVolumeMusicDefault(float);

            //! setter for default sfx volume
            void setVolumeSfxDefault(float);

            //! getter for shuffle music
            bool isShuffleMusicEnabled() const;

            //! setter for shuffle music
            void setShuffleMusicEnabled(bool value);

            //! getter for shuffle 1st track
            bool isShuffle1stTrackOnlyEnabled() const;

            //! setter for shuffle 1st track
            void setShuffle1stTrackOnlyEnabled(bool value);

            //! check if music player visible
            bool isMusicPlayerVisibile() const;

            //! setter for music player visible flag
            void setMusicPlayerVisibile(bool value);


      protected:

            // options

            //! sfx volume
            float mVolumeSfx;

            //! music volume
            float mVolumeMusic;

            //! sfx volume default
            float mVolumeSfxDefault;

            //! music volume default
            float mVolumeMusicDefault;

            //! checked if music is shuffled
            bool mShuffleMusic;

            //! checked if 1st track is shuffled
            bool mShuffle1stTrackOnly;

            //! checked if music player shall be shown
            bool mMusicPlayerVisible;
      };

      class LoginSettings : public SettingsPrivate
      {
         public:

            //! serialize audio settings
            void serialize();

            //! deserialize audio settings
            void deserialize();

            //! setter for nick
            void setNick(const QString& nick);

            //! getter for nick
            const QString& getNick() const;

            //! setter for host
            void setHost(const QString& host);

            //! getter for host
            const QString& getHost() const;

            //! getter for nick of player 2
            QString getPlayer2Nick() const;

            //! setter for nick of player 2
            void setPlayer2Nick(const QString &value);

            //! getter for nick of player 3
            QString getPlayer3Nick() const;

            //! setter for nick of player 3
            void setPlayer3Nick(const QString &value);

            //! getter for nick of player 4
            QString getPlayer4Nick() const;

            //! setter for nick of player 4
            void setPlayer4Nick(const QString &value);

            //! getter for nick of player 5
            QString getPlayer5Nick() const;

            //! setter for nick of player 5
            void setPlayer5Nick(const QString &value);

            //! getter for nick of player 6
            QString getPlayer6Nick() const;

            //! setter for nick of player 6
            void setPlayer6Nick(const QString &value);

            //! getter for nick of player 7
            QString getPlayer7Nick() const;

            //! setter for nick of player 7
            void setPlayer7Nick(const QString &value);

            //! getter for nick of player 8
            QString getPlayer8Nick() const;

            //! setter for nick of player 8
            void setPlayer8Nick(const QString &value);

            //! getter for nick of player 9
            QString getPlayer9Nick() const;

            //! setter for nick of player 9
            void setPlayer9Nick(const QString &value);

            //! getter for nick of player 10
            QString getPlayer10Nick() const;

            //! setter for nick of player 10
            void setPlayer10Nick(const QString &value);


      protected:

            //! login nick
            QString mNick;

            //! login host
            QString mHost;

            //! player 2 nick
            QString mPlayer2Nick;

            //! player 3 nick
            QString mPlayer3Nick;

            //! player 4 nick
            QString mPlayer4Nick;

            //! player 5 nick
            QString mPlayer5Nick;

            //! player 6 nick
            QString mPlayer6Nick;

            //! player 7 nick
            QString mPlayer7Nick;

            //! player 8 nick
            QString mPlayer8Nick;

            //! player 9 nick
            QString mPlayer9Nick;

            //! player 10 nick
            QString mPlayer10Nick;
      };


      class VideoSettings : public SettingsPrivate
      {
         public:

            //! constructor
            VideoSettings();

            //! serialize video settings
            void serialize();

            //! deserialize video settings
            void deserialize();

            //! backup video settings
            static void duplicate(VideoSettings* dest, VideoSettings* src);

            //! restore video settings
            void restoreDefaults();

            //! setter for video mode width
            void setWidth(int width);

            //! getter for video mode width
            int getWidth() const;

            //! setter for video mode height
            void setHeight(int height);

            //! getter for video mode height
            int getHeight() const;

            //! setter for video resolution
            void setResolution(int res);

            //! getter for video resolution
            int getResolution() const;

            //! setter for antialias samples
            void setAntialias(int samples);

            //! getter for antialias samples
            int getAntialias() const;

            //! setter for fullscreen mode
            void setFullscreen(bool fullscreen);

            //! getter for fullscreen mode
            bool isFullscreen() const;

            //! toggle fullscreen
            void toggleFullscreen();

            //! setter for brightness
            void setBrightness(float brightness);

            //! getter for brightness
            float getBrightness() const;

            //! check if vsync is enabled
            int getVSync() const;

            //! setter for vsync
            void setVSync(int enabled);

            //! getter for fps visibility
            bool isFpsShown() const;

            //! setter for fps visibility
            void setShowFps(bool value);

            //! get/set zoom factor (0.8..1.2  default: 1.0)
            float getZoom() const;
            void setZoom(float zoom);

            //! get/set border compensation (in pixels  default: 0)
            int getBorderLeft() const;
            void setBorderLeft(int left);
            int getBorderTop() const;
            void setBorderTop(int top);
            int getBorderRight() const;
            void setBorderRight(int right);
            int getBorderBottom() const;
            void setBorderBottom(int bottom);

         protected:

            //! window width
            int mWidth;

            //! window height
            int mHeight;

            //! rendering resolution (eg 1x1, 2x2, ...)
            int mResolution;

            //! antialias samples
            int mAntialias;

            //! fullscreen flag
            bool mFullscreen;

            //! brightness
            float mBrightness;

            //! vsync
            int mVSync;

            //! show fps
            bool mShowFps;

            //! camera zoom factor
            float mZoom;

            //! screen border compensation
            int mBorderLeft;
            int mBorderTop;
            int mBorderRight;
            int mBorderBottom;
      };


      class ControllerSettings : public SettingsPrivate
      {
         public:

            //! constructor
            ControllerSettings();

            //! serialize controller settings
            void serialize();

            //! deserialize controller settings
            void deserialize();

            //! restore controller settings
            void restoreDefaults();

            //! setter for keymap
            void setKeyMap(const QMap<Constants::Key, int>& keyMap);

            //! getter for keymap
            QMap<Constants::Key, int> getKeyMap() const;

            Qt::Key getUpKey() const;
            Qt::Key getDownKey() const;
            Qt::Key getLeftKey() const;
            Qt::Key getRightKey() const;
            Qt::Key getBombKey() const;
            Qt::Key getZoomOutKey() const;
            Qt::Key getZoomInKey() const;
            Qt::Key getStartKey() const;

            //! setter for analogue axis 1
            void setAnalogueAxis1(int axis1);

            //! getter for analogue axis 1
            int getAnalogueAxis1() const;

            //! setter for analogue axis 2
            void setAnalogueAxis2(int axis2);

            //! getter for analogue axis 2
            int getAnalogueAxis2() const;

            //! setter for analogue sensitivity
            void setAnalogueThreshold(int);

            //! getter for analogue sensitivity
            int getAnalogueThreshold() const;


         protected:

            //! initialize default map
            void initializeDefaultMap();

            //! keymap
            QMap<Constants::Key, int> mKeyMap;

            //! analogue axis 1
            int mAnalogueAxis1;

            //! analogue axis 2
            int mAnalogueAxis2;

            //! analogue sensitivity
            int mAnalogueTreshold;
      };


      class CreateGameSettings : public SettingsPrivate
      {
         public:

            //! constructor
            CreateGameSettings();

            //! serialize creategame settings
            void serialize();

            //! deserialize creategame settings
            void deserialize();

            //! setter for game name
            void setGameName(const QString&);

            //! getter for game name
            const QString& getGameName() const;

            //! setter for level index
            void setLevelIndex(int index);

            //! getter for level index
            int getLevelIndex() const;

            //! setter for number of rounds
            void setRounds(int);

            //! getter for number of rounds
            int getRounds() const;

            //! setter for game duration
            void setDuration(int);

            //! getter for game duration
            int getDuration() const;

            //! setter for bot count
            void setBotCount(int val);

            //! getter for bot count
            int getBotCount() const;

            //! setter for maximum player count
            void setMaxPlayers(int);

            //! getter for maximum player count
            int getMaxPlayers() const;

            //! setter for bombs extra flag
            void setExtraBombsEnabled(bool);

            //! getter for bombs extra flag
            bool isExtraBombsEnabled() const;

            //! setter for flames extra flag
            void setExtraFlamesEnabled(bool);

            //! getter for bombs extra flag
            bool isExtraFlamesEnabled() const;

            //! setter for speed-up extra flag
            void setExtraSpeedUpsEnabled(bool);

            //! setter for skulls extra flag
            void setExtraSkullsEnabled(bool);

            //! getter for bombs extra flag
            bool isExtraSpeedUpsEnabled() const;

            //! setter for kick extra flag
            void setExtraKicksEnabled(bool);

            //! getter for bombs extra flag
            bool isExtraKicksEnabled() const;

            //! getter for skulls extra flag
            bool isExtraSkullsEnabled() const;

            //! setter for dimensions
            void setDimensions(Constants::Dimension);

            //! getter for dimensions
            Constants::Dimension getDimensions() const;

            //! getter for single player flag
            bool isSinglePlayer() const;

            //! setter for single player flag
            void setSinglePlayer(bool value);


      protected:

            //! single player flag
            bool mSinglePlayer;

            //! game name
            QString mGameName;

            //! level index
            int mLevelIndex;

            //! number of rounds
            int mRounds;

            //! duration
            int mDuration;

            //! maximum player count
            int mMaxPlayers;

            //! bot count
            int mBotCount;

            //! extra flag: bombs
            bool mExtraBombs;

            //! extra flag: flames
            bool mExtraFlames;

            //! extra flag: speedups
            bool mExtraSpeedUps;

            //! extra flag: kicks
            bool mExtraKicks;

            //! extra flag: skulls
            bool mExtraSkulls;

            //! dimensions
            Constants::Dimension mDimensions;
      };


      //! destructor
      virtual ~GameSettings();

      //! instance getter
      static GameSettings* getInstance();

      //! getter for development settings
      DevelopmentSettings* getDevelopmentSettings();

      //! getter for style settings
      StyleSettings* getStyleSettings();

      //! getter for gameplay settings
      GameplaySettings* getGameplaySettings();

      //! getter for audio settings
      AudioSettings* getAudioSettings();

      //! getter for login settings
      LoginSettings* getLoginSettings();

      //! getter for video settings
      VideoSettings* getVideoSettings();

      //! getter for video settings backup
      VideoSettings* getVideoSettingsBackup();

      //! getter for creategame settings
      CreateGameSettings* getCreateGameSettingsSingle();

      //! getter for creategame settings
      CreateGameSettings* getCreateGameSettingsMulti();

      //! getter for controller settings
      ControllerSettings* getControllerSettings();

      //! store all options (audio/video/controller)
      void serialize();


   protected:

      //! constructor
      GameSettings();

      //! development setting
      DevelopmentSettings* mDevelopmentSettings;

      //! audio settings
      AudioSettings* mAudioSettings;

      //! gameplay settings
      GameplaySettings* mGameplaySettings;

      //! login settings
      LoginSettings* mLoginSettings;

      //! video settings
      VideoSettings* mVideoSettings;

      //! video settings backup
      VideoSettings* mVideoSettingsBackup;

      //! controller settings
      ControllerSettings* mControllerSettings;

      //! creategame settings for single player
      CreateGameSettings* mCreateGameSettingsSingle;

      //! creategame settings for multi player
      CreateGameSettings* mCreateGameSettingsMulti;

      //! design settings
      StyleSettings* mStyleSettings;

      //! singleton instance
      static GameSettings* sSettings;

      //! list of settings
      QList<SettingsPrivate*> mSettings;
};

#endif // GAMESETTINGS_H

