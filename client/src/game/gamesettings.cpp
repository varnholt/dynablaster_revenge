#include "gamesettings.h"

#define SETTINGS_FILE "data/game.ini"

// defaults

// audio
#define DEFAULT_VOLUME_SFX          0.5f
#define DEFAULT_VOLUME_MUSIC        0.5f

// video
#define DEFAULT_VIDEO_WIDTH          1024
#define DEFAULT_VIDEO_HEIGHT         576
#define DEFAULT_VIDEO_RESOLUTION     1
#define DEFAULT_VIDEO_ANTIALIAS      1
#define DEFAULT_VIDEO_FULLSCREEN     false
#define DEFAULT_VIDEO_BRIGHTNESS     0.5
#define DEFAULT_VIDEO_VSYNC          1
#define DEFAULT_VIDEO_SHOWFPS        false
#define DEFAULT_VIDEO_ZOOM           1
#define DEFAULT_VIDEO_BORDERLEFT     0
#define DEFAULT_VIDEO_BORDERTOP      0
#define DEFAULT_VIDEO_BORDERRIGHT    0
#define DEFAULT_VIDEO_BORDERBOTTOM   0

// control
#define KEYMAP_NOMINAL_SIZE          5
#define DEFAULT_ANALOGUE_AXIS_1      0
#define DEFAULT_ANALOGUE_AXIS_2      1
#define DEFAULT_ANALOGUE_THRESHOLD 3200

// gameplay
#define DEFAULT_GAMEPLAY_CAMERA_SHAKE_INTENSITY 1.0f
#define DEFAULT_GAMEPLAY_CAMERA_FOLLOWS_PLAYER  true


GameSettings* GameSettings::sSettings = nullptr;


GameSettings::GameSettings()
 : mDevelopmentSettings(nullptr),
   mAudioSettings(nullptr),
   mGameplaySettings(nullptr),
   mLoginSettings(nullptr),
   mVideoSettings(nullptr),
   mVideoSettingsBackup(nullptr),
   mControllerSettings(nullptr),
   mCreateGameSettingsSingle(nullptr),
   mCreateGameSettingsMulti(nullptr),
   mStyleSettings(nullptr)
{
   sSettings = this;

   // create settings instances
   mDevelopmentSettings = new DevelopmentSettings();
   mAudioSettings = new AudioSettings();
   mGameplaySettings = new GameplaySettings();
   mLoginSettings = new LoginSettings();
   mVideoSettings = new VideoSettings();
   mVideoSettingsBackup = new VideoSettings();
   mControllerSettings = new ControllerSettings;
   mCreateGameSettingsSingle = new CreateGameSettings();
   mCreateGameSettingsMulti = new CreateGameSettings();
   mStyleSettings = new StyleSettings();

   mCreateGameSettingsSingle->setSinglePlayer(true);
   mCreateGameSettingsMulti->setSinglePlayer(false);

   mSettings << mDevelopmentSettings;
   mSettings << mAudioSettings;
   mSettings << mGameplaySettings;
   mSettings << mLoginSettings;
   mSettings << mVideoSettings;
   mSettings << mControllerSettings;
   mSettings << mCreateGameSettingsSingle;
   mSettings << mCreateGameSettingsMulti;
   mSettings << mStyleSettings;

   // deserialize settings data
   foreach(SettingsPrivate* settings, mSettings)
      settings->deserialize();
}


GameSettings::~GameSettings()
{
   delete mDevelopmentSettings;
   delete mAudioSettings;
   delete mGameplaySettings;
   delete mLoginSettings;
   delete mVideoSettings;
   delete mControllerSettings;
   delete mCreateGameSettingsSingle;
   delete mCreateGameSettingsMulti;
   delete mStyleSettings;

   mDevelopmentSettings = nullptr;
   mAudioSettings = nullptr;
   mGameplaySettings = nullptr;
   mLoginSettings = nullptr;
   mVideoSettings = nullptr;
   mControllerSettings = nullptr;
   mCreateGameSettingsSingle = nullptr;
   mCreateGameSettingsMulti = nullptr;
   mStyleSettings = nullptr;
}


GameSettings* GameSettings::getInstance()
{
   if (!sSettings)
      new GameSettings();

   return sSettings;
}


GameSettings::SettingsPrivate::SettingsPrivate()
 : QSettings(
      SETTINGS_FILE,
      QSettings::IniFormat
   )
{
}


GameSettings::SettingsPrivate::~SettingsPrivate()
{
}


void GameSettings::SettingsPrivate::deserialize()
{
}


void GameSettings::SettingsPrivate::serialize()
{
}


void GameSettings::SettingsPrivate::restoreDefaults()
{
}


GameSettings::DevelopmentSettings *GameSettings::getDevelopmentSettings()
{
   return mDevelopmentSettings;
}


GameSettings::StyleSettings *GameSettings::getStyleSettings()
{
   return mStyleSettings;
}


GameSettings::GameplaySettings *GameSettings::getGameplaySettings()
{
   return mGameplaySettings;
}


GameSettings::AudioSettings *GameSettings::getAudioSettings()
{
   return mAudioSettings;
}


GameSettings::LoginSettings *GameSettings::getLoginSettings()
{
   return mLoginSettings;
}


GameSettings::VideoSettings *GameSettings::getVideoSettings()
{
   return mVideoSettings;
}


GameSettings::VideoSettings *GameSettings::getVideoSettingsBackup()
{
   return mVideoSettingsBackup;
}


GameSettings::CreateGameSettings *GameSettings::getCreateGameSettingsSingle()
{
   return mCreateGameSettingsSingle;
}


GameSettings::CreateGameSettings *GameSettings::getCreateGameSettingsMulti()
{
   return mCreateGameSettingsMulti;
}


GameSettings::ControllerSettings *GameSettings::getControllerSettings()
{
   return mControllerSettings;
}


void GameSettings::serialize()
{
   getAudioSettings()->serialize();
   getVideoSettings()->serialize();
   getControllerSettings()->serialize();
   getGameplaySettings()->serialize();
}


GameSettings::AudioSettings::AudioSettings()
 : mVolumeSfx(0.0f),
   mVolumeMusic(0.0f),
   mVolumeSfxDefault(0.0f),
   mVolumeMusicDefault(0.0f),
   mShuffleMusic(true),
   mShuffle1stTrackOnly(false),
   mMusicPlayerVisible(true)
{
}


void GameSettings::AudioSettings::serialize()
{
   setValue("audio/volume_music", getVolumeMusic());
   setValue("audio/volume_sfx", getVolumeSfx());
}


void GameSettings::AudioSettings::deserialize()
{
   bool ok = false;

   float volume = 0.0f;

   volume = value("audio/volume_music").toFloat(&ok);

   setVolumeMusic(
      ok
         ? volume
         : DEFAULT_VOLUME_MUSIC
   );

   volume = value("audio/volume_sfx").toFloat(&ok);

   setVolumeSfx(
      ok
         ? volume
         : DEFAULT_VOLUME_SFX
   );

   volume = value("audio/volume_music_default").toFloat(&ok);

   setVolumeMusicDefault(
      ok
         ? volume
         : DEFAULT_VOLUME_MUSIC
   );

   volume = value("audio/volume_sfx_default").toFloat(&ok);

   setVolumeSfxDefault(
      ok
         ? volume
         : DEFAULT_VOLUME_SFX
   );

   bool shuffleMusic = value("audio/shuffle_music", true).toBool();
   bool shuffle1stTrackOnly = value("audio/shuffle_1st_track_only", false).toBool();

   setShuffleMusicEnabled(shuffleMusic);
   setShuffle1stTrackOnlyEnabled(shuffle1stTrackOnly);

   bool musicPlayerVisible = value("audio/musicplayer_visible", true).toBool();
   setMusicPlayerVisibile(musicPlayerVisible);
}


void GameSettings::AudioSettings::restoreDefaults()
{
   SettingsPrivate::restoreDefaults();

   setVolumeMusic(DEFAULT_VOLUME_MUSIC);
   setVolumeSfx(DEFAULT_VOLUME_SFX);
}


float GameSettings::AudioSettings::getVolumeMusic() const
{
   return mVolumeMusic;
}


float GameSettings::AudioSettings::getVolumeSfx() const
{
   return mVolumeSfx;
}


void GameSettings::AudioSettings::setVolumeMusic(float volume)
{
   mVolumeMusic = volume;
}


void GameSettings::AudioSettings::setVolumeSfx(float volume)
{
   mVolumeSfx = volume;
}


float GameSettings::AudioSettings::getVolumeMusicDefault() const
{
   return mVolumeMusicDefault;
}


float GameSettings::AudioSettings::getVolumeSfxDefault() const
{
   return mVolumeSfxDefault;
}


void GameSettings::AudioSettings::setVolumeMusicDefault(float volume)
{
   mVolumeMusicDefault = volume;
}


void GameSettings::AudioSettings::setVolumeSfxDefault(float volume)
{
   mVolumeSfxDefault = volume;
}


bool GameSettings::AudioSettings::isShuffleMusicEnabled() const
{
   return mShuffleMusic;
}


void GameSettings::AudioSettings::setShuffleMusicEnabled(bool value)
{
   mShuffleMusic = value;
}


bool GameSettings::AudioSettings::isShuffle1stTrackOnlyEnabled() const
{
   return mShuffle1stTrackOnly;
}


void GameSettings::AudioSettings::setShuffle1stTrackOnlyEnabled(bool value)
{
   mShuffle1stTrackOnly = value;
}


bool GameSettings::AudioSettings::isMusicPlayerVisibile() const
{
   return mMusicPlayerVisible;
}


void GameSettings::AudioSettings::setMusicPlayerVisibile(bool value)
{
   mMusicPlayerVisible = value;
}


GameSettings::DevelopmentSettings::DevelopmentSettings()
 : mSkipMenu(false),
   mShowSplash(false),
   mMusicEnabled(false),
   mDryRunEnabled(false),
   mGameRecordingEnabled(false),
   mJoysticksEnabled(true)
{
}


void GameSettings::DevelopmentSettings::deserialize()
{
   // init config
   mSkipMenu = value("development/skipmenu", false).toBool();
   mShowSplash = value("development/showsplash", true).toBool();
   mMusicEnabled = value("development/music", true).toBool();
   mLevel = value("development/level", "castle").toString();
   mDryRunEnabled = value("development/dryrun", false).toBool();
   mGameRecordingEnabled = value("development/gamerecording", false).toBool();
   mJoysticksEnabled = value("development/joysticksenabled", true).toBool();

   setPageFacebook(
      value(
         "development/page_facebook",
         "https://www.facebook.com/DynablasterRevenge"
      ).toString()
   );

   setPagePouet(
      value(
         "development/page_pouet",
         "http://www.pouet.net/prod.php?which=62926"
      ).toString()
   );

   setPageHome(
      value(
         "development/page_home",
         "https://dynablaster.titandemo.de/"
      ).toString()
   );
}


bool GameSettings::DevelopmentSettings::isSkipMenuEnabled()
{
   return mSkipMenu;
}


bool GameSettings::DevelopmentSettings::isSplashScreenEnabled()
{
   return mShowSplash;
}


bool GameSettings::DevelopmentSettings::isMusicEnabled()
{
   return mMusicEnabled;
}


const QString &GameSettings::DevelopmentSettings::getLevel() const
{
   return mLevel;
}


bool GameSettings::DevelopmentSettings::isDryRunEnabled() const
{
   return mDryRunEnabled;
}


void GameSettings::DevelopmentSettings::setDryRunEnabled(bool enabled)
{
   mDryRunEnabled = enabled;
}


bool GameSettings::DevelopmentSettings::isGameRecordingEnabled() const
{
   return mGameRecordingEnabled;
}


void GameSettings::DevelopmentSettings::setGameRecordingEnabled(bool value)
{
   mGameRecordingEnabled = value;
}


bool GameSettings::DevelopmentSettings::isJoysticksEnabled() const
{
   return mJoysticksEnabled;
}


void GameSettings::DevelopmentSettings::setJoysticksEnabled(bool value)
{
   mJoysticksEnabled = value;
}


QString GameSettings::DevelopmentSettings::getPageHome() const
{
    return mPageHome;
}


void GameSettings::DevelopmentSettings::setPageHome(const QString &value)
{
    mPageHome = value;
}


QString GameSettings::DevelopmentSettings::getPageFacebook() const
{
    return mPageFacebook;
}


void GameSettings::DevelopmentSettings::setPageFacebook(const QString &value)
{
    mPageFacebook = value;
}


QString GameSettings::DevelopmentSettings::getPagePouet() const
{
    return mPagePouet;
}


void GameSettings::DevelopmentSettings::setPagePouet(const QString &value)
{
    mPagePouet = value;
}


void GameSettings::LoginSettings::serialize()
{
    setValue("logindata/nick", getNick());
    setValue("logindata/host", getHost());
}


void GameSettings::LoginSettings::deserialize()
{
   setNick(value("logindata/nick").toString());
   setHost(value("logindata/host").toString());

   // player n nicks
   setPlayer2Nick(value("logindata/player2", "player2").toString());
   setPlayer3Nick(value("logindata/player3", "player3").toString());
   setPlayer4Nick(value("logindata/player4", "player4").toString());
   setPlayer5Nick(value("logindata/player5", "player5").toString());
   setPlayer6Nick(value("logindata/player6", "player6").toString());
   setPlayer7Nick(value("logindata/player7", "player7").toString());
   setPlayer8Nick(value("logindata/player8", "player8").toString());
   setPlayer9Nick(value("logindata/player9", "player9").toString());
   setPlayer10Nick(value("logindata/player10", "player10").toString());
}


void GameSettings::LoginSettings::setNick(const QString &nick)
{
   mNick = nick;
}


const QString &GameSettings::LoginSettings::getNick() const
{
   return mNick;
}


void GameSettings::LoginSettings::setHost(const QString &host)
{
   mHost = host;
}


const QString &GameSettings::LoginSettings::getHost() const
{
   return mHost;
}


QString GameSettings::LoginSettings::getPlayer10Nick() const
{
   return mPlayer10Nick;
}


void GameSettings::LoginSettings::setPlayer10Nick(const QString &value)
{
   mPlayer10Nick = value;
}


QString GameSettings::LoginSettings::getPlayer9Nick() const
{
   return mPlayer9Nick;
}


void GameSettings::LoginSettings::setPlayer9Nick(const QString &value)
{
   mPlayer9Nick = value;
}


QString GameSettings::LoginSettings::getPlayer8Nick() const
{
   return mPlayer8Nick;
}


void GameSettings::LoginSettings::setPlayer8Nick(const QString &value)
{
   mPlayer8Nick = value;
}


QString GameSettings::LoginSettings::getPlayer7Nick() const
{
   return mPlayer7Nick;
}


void GameSettings::LoginSettings::setPlayer7Nick(const QString &value)
{
   mPlayer7Nick = value;
}


QString GameSettings::LoginSettings::getPlayer6Nick() const
{
   return mPlayer6Nick;
}


void GameSettings::LoginSettings::setPlayer6Nick(const QString &value)
{
   mPlayer6Nick = value;
}


QString GameSettings::LoginSettings::getPlayer5Nick() const
{
   return mPlayer5Nick;
}


void GameSettings::LoginSettings::setPlayer5Nick(const QString &value)
{
   mPlayer5Nick = value;
}


QString GameSettings::LoginSettings::getPlayer4Nick() const
{
   return mPlayer4Nick;
}


void GameSettings::LoginSettings::setPlayer4Nick(const QString &value)
{
   mPlayer4Nick = value;
}


QString GameSettings::LoginSettings::getPlayer3Nick() const
{
   return mPlayer3Nick;
}


void GameSettings::LoginSettings::setPlayer3Nick(const QString &value)
{
   mPlayer3Nick = value;
}


QString GameSettings::LoginSettings::getPlayer2Nick() const
{
   return mPlayer2Nick;
}


void GameSettings::LoginSettings::setPlayer2Nick(const QString &value)
{
   mPlayer2Nick = value;
}



GameSettings::CreateGameSettings::CreateGameSettings()
 : mSinglePlayer(false),
   mRounds(0),
   mDuration(0),
   mMaxPlayers(0),
   mBotCount(0),
   mExtraBombs(false),
   mExtraFlames(false),
   mExtraSpeedUps(false),
   mExtraKicks(false),
   mExtraSkulls(false),
   mDimensions(Constants::DimensionInvalid)
{

}


void GameSettings::CreateGameSettings::serialize()
{
   if (isSinglePlayer())
   {
      setValue("creategamesingle/gamename", getGameName());
      setValue("creategamesingle/levelindex", getLevelIndex());
      setValue("creategamesingle/rounds", getRounds());
      setValue("creategamesingle/duration", getDuration());
      setValue("creategamesingle/maxplayers", getMaxPlayers());
      setValue("creategamesingle/botcount", getBotCount());
      setValue("creategamesingle/extrabombs", isExtraBombsEnabled());
      setValue("creategamesingle/extraflames", isExtraFlamesEnabled());
      setValue("creategamesingle/extrakicks", isExtraKicksEnabled());
      setValue("creategamesingle/extraspeedups", isExtraSpeedUpsEnabled());
      setValue("creategamesingle/extraskulls", isExtraSkullsEnabled());
      setValue("creategamesingle/dimension", getDimensions());
   }
   else
   {
      setValue("creategamemulti/gamename", getGameName());
      setValue("creategamemulti/levelindex", getLevelIndex());
      setValue("creategamemulti/rounds", getRounds());
      setValue("creategamemulti/duration", getDuration());
      setValue("creategamemulti/maxplayers", getMaxPlayers());
      setValue("creategamemulti/botcount", getBotCount());
      setValue("creategamemulti/extrabombs", isExtraBombsEnabled());
      setValue("creategamemulti/extraflames", isExtraFlamesEnabled());
      setValue("creategamemulti/extrakicks", isExtraKicksEnabled());
      setValue("creategamemulti/extraspeedups", isExtraSpeedUpsEnabled());
      setValue("creategamemulti/extraskulls", isExtraSkullsEnabled());
      setValue("creategamemulti/dimension", getDimensions());
   }
}


void GameSettings::CreateGameSettings::deserialize()
{
   if (isSinglePlayer())
   {
      setGameName(value("creategamesingle/gamename", tr("Default")).toString());
      setLevelIndex(value("creategamesingle/levelindex", 0).toInt());
      setRounds(value("creategamesingle/rounds", 1).toInt());
      setDuration(value("creategamesingle/duration", 3).toInt());
      setMaxPlayers(value("creategamesingle/maxplayers", 5).toInt());
      setBotCount(value("creategamesingle/botcount", 4).toInt());

      setExtraBombsEnabled(value("creategamesingle/extrabombs", true).toBool());
      setExtraFlamesEnabled(value("creategamesingle/extraflames", true).toBool());
      setExtraKicksEnabled(value("creategamesingle/extrakicks", false).toBool());
      setExtraSpeedUpsEnabled(value("creategamesingle/extraspeedups", false).toBool());
      setExtraSkullsEnabled(value("creategamesingle/extraskulls", false).toBool());

      setDimensions(
         static_cast<Constants::Dimension>(value(
            "creategamesingle/dimension",
            static_cast<int32_t>(Constants::Dimension13x11)
         ).toInt()));
   }
   else
   {
      setGameName(value("creategamemulti/gamename", tr("Default")).toString());
      setLevelIndex(value("creategamemulti/levelindex", 0).toInt());
      setRounds(value("creategamemulti/rounds", 1).toInt());
      setDuration(value("creategamemulti/duration", 3).toInt());
      setMaxPlayers(value("creategamemulti/maxplayers", 5).toInt());
      setBotCount(value("creategamemulti/botcount", 0).toInt());

      setExtraBombsEnabled(value("creategamemulti/extrabombs", true).toBool());
      setExtraFlamesEnabled(value("creategamemulti/extraflames", true).toBool());
      setExtraKicksEnabled(value("creategamemulti/extrakicks", false).toBool());
      setExtraSpeedUpsEnabled(value("creategamemulti/extraspeedups", false).toBool());
      setExtraSkullsEnabled(value("creategamemulti/extraskulls", false).toBool());

      setDimensions(
         static_cast<Constants::Dimension>(value(
            "creategamemulti/dimension",
            static_cast<int32_t>(Constants::Dimension13x11)
         ).toInt()));
   }
}


void GameSettings::CreateGameSettings::setGameName(const QString & val)
{
   mGameName = val;
}


const QString &GameSettings::CreateGameSettings::getGameName() const
{
   return mGameName;
}


void GameSettings::CreateGameSettings::setLevelIndex(int index)
{
   mLevelIndex = index;
}


int GameSettings::CreateGameSettings::getLevelIndex() const
{
   return mLevelIndex;
}


void GameSettings::CreateGameSettings::setRounds(int rounds)
{
   mRounds = rounds;
}


int GameSettings::CreateGameSettings::getRounds() const
{
   return mRounds;
}


void GameSettings::CreateGameSettings::setDuration(int duration)
{
   mDuration = duration;
}


int GameSettings::CreateGameSettings::getDuration() const
{
   return mDuration;
}


void GameSettings::CreateGameSettings::setBotCount(int val)
{
   mBotCount = val;
}


int GameSettings::CreateGameSettings::getBotCount() const
{
   return mBotCount;
}


void GameSettings::CreateGameSettings::setMaxPlayers(int val)
{
   mMaxPlayers = val;
}


int GameSettings::CreateGameSettings::getMaxPlayers() const
{
   return mMaxPlayers;
}


void GameSettings::CreateGameSettings::setExtraBombsEnabled(bool enabled)
{
   mExtraBombs = enabled;
}


bool GameSettings::CreateGameSettings::isExtraBombsEnabled() const
{
   return mExtraBombs;
}


void GameSettings::CreateGameSettings::setExtraFlamesEnabled(bool enabled)
{
   mExtraFlames = enabled;
}


bool GameSettings::CreateGameSettings::isExtraFlamesEnabled() const
{
   return mExtraFlames;
}


void GameSettings::CreateGameSettings::setExtraSpeedUpsEnabled(bool enabled)
{
   mExtraSpeedUps = enabled;
}


bool GameSettings::CreateGameSettings::isExtraSpeedUpsEnabled() const
{
   return mExtraSpeedUps;
}


void GameSettings::CreateGameSettings::setExtraKicksEnabled(bool enabled)
{
   mExtraKicks = enabled;
}


bool GameSettings::CreateGameSettings::isExtraKicksEnabled() const
{
   return mExtraKicks;
}


void GameSettings::CreateGameSettings::setExtraSkullsEnabled(bool enabled)
{
   mExtraSkulls = enabled;
}


bool GameSettings::CreateGameSettings::isExtraSkullsEnabled() const
{
   return mExtraSkulls;
}


void GameSettings::CreateGameSettings::setDimensions(Constants::Dimension dimensions)
{
   mDimensions = dimensions;
}


Constants::Dimension GameSettings::CreateGameSettings::getDimensions() const
{
   return mDimensions;
}


bool GameSettings::CreateGameSettings::isSinglePlayer() const
{
   return mSinglePlayer;
}


void GameSettings::CreateGameSettings::setSinglePlayer(bool value)
{
   mSinglePlayer = value;
}


GameSettings::ControllerSettings::ControllerSettings()
   : mAnalogueAxis1(DEFAULT_ANALOGUE_AXIS_1),
     mAnalogueAxis2(DEFAULT_ANALOGUE_AXIS_2),
     mAnalogueTreshold(DEFAULT_ANALOGUE_THRESHOLD)
{
}


void GameSettings::ControllerSettings::serialize()
{
   if (mKeyMap.size() == KEYMAP_NOMINAL_SIZE)
   {
      SettingsMap serializeMap;

      QMapIterator<Constants::Key, int> i(mKeyMap);
      while (i.hasNext())
      {
         i.next();
         serializeMap.insert(
            QString("%1").arg(i.key()),
            QString("%1").arg(i.value())
         );
      }

      setValue("controller/keymap", serializeMap);
   }

   setValue("controller/analogueaxis1", getAnalogueAxis1());
   setValue("controller/analogueaxis2", getAnalogueAxis2());
   setValue("controller/analoguethreshold", getAnalogueThreshold());
}


void GameSettings::ControllerSettings::deserialize()
{
   mKeyMap.clear();
   initializeDefaultMap();

   SettingsMap deserializeMap = value("controller/keymap").toMap();

   QMapIterator<QString, QVariant> i(deserializeMap);

   bool keyOk = false;
   bool keyValOk = false;

   while (i.hasNext())
   {
      i.next();

      Constants::Key key = static_cast<Constants::Key>(i.key().toInt(&keyOk));
      int keyVal = i.value().toInt(&keyValOk);

      if (keyOk && keyValOk)
      {
         if (mKeyMap.contains(key))
            mKeyMap.remove(key);

         mKeyMap.insert(
            key,
            keyVal
         );
      }
   }

   setAnalogueAxis1(
      value(
         "controller/analogueaxis1",
         DEFAULT_ANALOGUE_AXIS_1
      ).toInt()
   );

   setAnalogueAxis2(
      value(
         "controller/analogueaxis2",
         DEFAULT_ANALOGUE_AXIS_2
      ).toInt()
   );

   setAnalogueThreshold(
      value(
         "controller/analoguethreshold",
         DEFAULT_ANALOGUE_THRESHOLD
      ).toInt()
   );
}


void GameSettings::ControllerSettings::restoreDefaults()
{
   SettingsPrivate::restoreDefaults();

   mKeyMap.clear();
   initializeDefaultMap();

   setAnalogueAxis1(DEFAULT_ANALOGUE_AXIS_1);
   setAnalogueAxis2(DEFAULT_ANALOGUE_AXIS_2);
   setAnalogueThreshold(DEFAULT_ANALOGUE_THRESHOLD);
}


void GameSettings::ControllerSettings::setKeyMap(const QMap<Constants::Key, int> &keyMap)
{
   mKeyMap = keyMap;
}


QMap<Constants::Key, int> GameSettings::ControllerSettings::getKeyMap() const
{
   return mKeyMap;
}


void GameSettings::ControllerSettings::initializeDefaultMap()
{
   mKeyMap.insert(Constants::KeyUp,     Qt::Key_Up);
   mKeyMap.insert(Constants::KeyDown,   Qt::Key_Down);
   mKeyMap.insert(Constants::KeyLeft,   Qt::Key_Left);
   mKeyMap.insert(Constants::KeyRight,  Qt::Key_Right);
   mKeyMap.insert(Constants::KeyBomb,   Qt::Key_Space);
   mKeyMap.insert(Constants::KeyZoomIn, Qt::Key_BracketRight);
   mKeyMap.insert(Constants::KeyZoomOut,Qt::Key_BracketLeft);
   mKeyMap.insert(Constants::KeyStart,  Qt::Key_F10);
}


Qt::Key GameSettings::ControllerSettings::getUpKey() const
{
   return static_cast<Qt::Key>(mKeyMap[Constants::KeyUp]);
}


Qt::Key GameSettings::ControllerSettings::getDownKey() const
{
   return static_cast<Qt::Key>(mKeyMap[Constants::KeyDown]);
}


Qt::Key GameSettings::ControllerSettings::getLeftKey() const
{
   return static_cast<Qt::Key>(mKeyMap[Constants::KeyLeft]);
}


Qt::Key GameSettings::ControllerSettings::getRightKey() const
{
   return static_cast<Qt::Key>(mKeyMap[Constants::KeyRight]);
}


Qt::Key GameSettings::ControllerSettings::getBombKey() const
{
   return static_cast<Qt::Key>(mKeyMap[Constants::KeyBomb]);
}


Qt::Key GameSettings::ControllerSettings::getZoomInKey() const
{
   return static_cast<Qt::Key>(mKeyMap[Constants::KeyZoomIn]);
}


Qt::Key GameSettings::ControllerSettings::getZoomOutKey() const
{
   return static_cast<Qt::Key>(mKeyMap[Constants::KeyZoomOut]);
}


Qt::Key GameSettings::ControllerSettings::getStartKey() const
{
   return static_cast<Qt::Key>(mKeyMap[Constants::KeyStart]);
}


void GameSettings::ControllerSettings::setAnalogueAxis1(int axis1)
{
   mAnalogueAxis1 = axis1;
}


int GameSettings::ControllerSettings::getAnalogueAxis1() const
{
   return mAnalogueAxis1;
}


void GameSettings::ControllerSettings::setAnalogueAxis2(int axis2)
{
   mAnalogueAxis2 = axis2;
}


int GameSettings::ControllerSettings::getAnalogueAxis2() const
{
   return mAnalogueAxis2;
}


void GameSettings::ControllerSettings::setAnalogueThreshold(int threshold)
{
   mAnalogueTreshold = threshold;
}


int GameSettings::ControllerSettings::getAnalogueThreshold() const
{
   return mAnalogueTreshold;
}



GameSettings::StyleSettings::StyleSettings()
{
}


void GameSettings::StyleSettings::deserialize()
{
   mPlayerColors.clear();

   SettingsMap deserializeMap = value("style/colormap").toMap();

   // if nothing was found, init with default data
   if (deserializeMap.isEmpty())
   {
      initDefaultMap();
      serialize();
   }
   else
   {
      QMapIterator<QString, QVariant> i(deserializeMap);

      bool keyOk = false;

      while (i.hasNext())
      {
         i.next();

         Constants::Color key = static_cast<Constants::Color>(i.key().toInt(&keyOk));
         QColor value = i.value().toString();

         if (keyOk)
         {
            mPlayerColors.insert(
               key,
               value
            );
         }
      }
   }

   initializeIndividualColor();
}


void GameSettings::StyleSettings::initializeIndividualColor()
{
   QMap<Constants::Color, QColor>::const_iterator i = mPlayerColors.constBegin();

   QRgb rgb;
   while (i != mPlayerColors.constEnd())
   {
      rgb = i.value().rgb();

      // cout << i.key() << ": " << i.value() << endl;

      switch (i.key())
      {
         case Constants::ColorWhite:
            mColorWhite = rgb;
            break;
         case Constants::ColorBlack:
            mColorBlack = rgb;
            break;
         case Constants::ColorRed:
            mColorRed = rgb;
            break;
         case Constants::ColorGreen:
            mColorGreen = rgb;
            break;
         case Constants::ColorBlue:
            mColorBlue = rgb;
            break;
         case Constants::ColorGrey:
            mColorGrey = rgb;
            break;
         case Constants::ColorYellow:
            mColorYellow = rgb;
            break;
         case Constants::ColorPurple:
            mColorPurple = rgb;
            break;
         case Constants::ColorCyan:
            mColorCyan = rgb;
            break;
         case Constants::ColorOrange:
            mColorOrange = rgb;
            break;
      }

      ++i;
   }
}


void GameSettings::StyleSettings::serialize()
{
   SettingsMap serializeMap;

   QMapIterator<Constants::Color, QColor> i(mPlayerColors);
   while (i.hasNext())
   {
      i.next();
      serializeMap.insert(
         QString("%1").arg(i.key()),
         i.value()
      );
   }

   setValue("style/colormap", serializeMap);
}


QColor GameSettings::StyleSettings::getColor(Constants::Color playerColor) const
{
   QColor color;

   switch (playerColor)
   {
      case Constants::ColorWhite:
         color = mColorWhite;
         break;
      case Constants::ColorBlack:
         color = mColorBlack;
         break;
      case Constants::ColorRed:
         color = mColorRed;
         break;
      case Constants::ColorGreen:
         color = mColorGreen;
         break;
      case Constants::ColorBlue:
         color = mColorBlue;
         break;
      case Constants::ColorGrey:
         color = mColorGrey;
         break;
      case Constants::ColorYellow:
         color = mColorYellow;
         break;
      case Constants::ColorPurple:
         color = mColorPurple;
         break;
      case Constants::ColorCyan:
         color = mColorCyan;
         break;
      case Constants::ColorOrange:
         color = mColorOrange;
         break;
   }

//   QMap<Constants::Color, QColor>::const_iterator iter =
//      mPlayerColors.constFind(playerColor);
//
//   if (iter != mPlayerColors.constEnd())
//      color = iter.value();

   return color;
}


QRgb GameSettings::StyleSettings::getRgb(Constants::Color playerColor) const
{
   QRgb rgb;

   switch (playerColor)
   {
      case Constants::ColorWhite:
         rgb = mColorWhite;
         break;
      case Constants::ColorBlack:
         rgb = mColorBlack;
         break;
      case Constants::ColorRed:
         rgb = mColorRed;
         break;
      case Constants::ColorGreen:
         rgb = mColorGreen;
         break;
      case Constants::ColorBlue:
         rgb = mColorBlue;
         break;
      case Constants::ColorGrey:
         rgb = mColorGrey;
         break;
      case Constants::ColorYellow:
         rgb = mColorYellow;
         break;
      case Constants::ColorPurple:
         rgb = mColorPurple;
         break;
      case Constants::ColorCyan:
         rgb = mColorCyan;
         break;
      case Constants::ColorOrange:
         rgb = mColorOrange;
         break;
   }

   return rgb;
}


void GameSettings::StyleSettings::initDefaultMap()
{
   mPlayerColors.insert(Constants::ColorWhite,  QColor("#ffffff"));
   mPlayerColors.insert(Constants::ColorBlack,  QColor("#0e0e0e"));
   mPlayerColors.insert(Constants::ColorRed,    QColor("#ff1b1b"));
   mPlayerColors.insert(Constants::ColorGreen,  QColor("#51fb07"));
   mPlayerColors.insert(Constants::ColorBlue,   QColor("#0072ff"));
   mPlayerColors.insert(Constants::ColorGrey,   QColor("#51fb07"));
   mPlayerColors.insert(Constants::ColorYellow, QColor("#8c8c8c"));
   mPlayerColors.insert(Constants::ColorPurple, QColor("#fdba02"));
   mPlayerColors.insert(Constants::ColorCyan,   QColor("#00e5ff"));
   mPlayerColors.insert(Constants::ColorOrange, QColor("#ff6000"));
}


/*
   Player 1	           #ffffff   white
   Player 2				  #0e0e0e   black
   Player 3				  #ff1b1b   red
   Player 4            #0072ff   blue
   Player 5	           #51fb07   green
   Player 6				  #8c8c8c
   Player 7				  #fdba02
   Player 8            #bd20fe
   Player 9	           #00e5ff
   Player 10           #ff6000
   Player Chat         #8bd9fe
   Player Left         #ff0000   #960707 optional
   Player Joined	     #11f105   #0b9500 optional
   Game Notification   #f6ff00
*/


GameSettings::VideoSettings::VideoSettings()
 : mWidth(-1),
   mHeight(-1),
   mResolution(1),
   mAntialias(1),
   mFullscreen(false),
   mBrightness(2.2f),
   mVSync(1),
   mShowFps(true),
   mZoom(1.0f),
   mBorderLeft(0),
   mBorderTop(0),
   mBorderRight(0),
   mBorderBottom(0)
{
}


void GameSettings::VideoSettings::serialize()
{
   setValue("video/width", getWidth());
   setValue("video/height", getHeight());
   setValue("video/resolution", getResolution());
   setValue("video/antialias", getAntialias());
   setValue("video/fullscreen", isFullscreen());
   setValue("video/brightness", getBrightness());
   setValue("video/vsync", getVSync());
   setValue("video/showfps", isFpsShown());
   setValue("video/zoom", getZoom());
   setValue("video/borderleft", getBorderLeft());
   setValue("video/bordertop", getBorderTop());
   setValue("video/borderright", getBorderRight());
   setValue("video/borderbottom", getBorderBottom());
}


void GameSettings::VideoSettings::deserialize()
{
   setWidth(value("video/width",               DEFAULT_VIDEO_WIDTH).toInt());
   setHeight(value("video/height",             DEFAULT_VIDEO_HEIGHT).toInt());
   setResolution(value("video/resolution",     DEFAULT_VIDEO_RESOLUTION).toInt());
   setAntialias(value("video/antialias",       DEFAULT_VIDEO_ANTIALIAS).toInt());
   setFullscreen(value("video/fullscreen",     DEFAULT_VIDEO_FULLSCREEN).toBool());
   setBrightness(value("video/brightness",     DEFAULT_VIDEO_BRIGHTNESS).toFloat());
   setVSync(value("video/vsync",               DEFAULT_VIDEO_VSYNC).toInt());
   setShowFps(value("video/showfps",           DEFAULT_VIDEO_SHOWFPS).toBool());
   setZoom(value("video/zoom",                 DEFAULT_VIDEO_ZOOM).toFloat());
   setBorderLeft(value("video/borderleft",     DEFAULT_VIDEO_BORDERLEFT).toInt());
   setBorderTop(value("video/bordertop",       DEFAULT_VIDEO_BORDERTOP).toInt());
   setBorderRight(value("video/borderright",   DEFAULT_VIDEO_BORDERRIGHT).toInt());
   setBorderBottom(value("video/borderbottom", DEFAULT_VIDEO_BORDERBOTTOM).toInt());
}


void GameSettings::VideoSettings::duplicate(
   GameSettings::VideoSettings * dest,
   GameSettings::VideoSettings * src
)
{
   dest->setWidth(src->getWidth());
   dest->setHeight(src->getHeight());
   dest->setResolution(src->getResolution());
   dest->setAntialias(src->getAntialias());
   dest->setFullscreen(src->isFullscreen());
   dest->setBrightness(src->getBrightness());
   dest->setVSync(src->getVSync());
   dest->setShowFps(src->isFpsShown());
   dest->setZoom(src->getZoom());
   dest->setBorderLeft(src->getBorderLeft());
   dest->setBorderTop(src->getBorderTop());
   dest->setBorderRight(src->getBorderRight());
   dest->setBorderBottom(src->getBorderBottom());
}


void GameSettings::VideoSettings::restoreDefaults()
{
   SettingsPrivate::restoreDefaults();

   setWidth(DEFAULT_VIDEO_WIDTH);
   setHeight(DEFAULT_VIDEO_HEIGHT);
   setResolution(DEFAULT_VIDEO_RESOLUTION);
   setAntialias(DEFAULT_VIDEO_ANTIALIAS);
   setFullscreen(DEFAULT_VIDEO_FULLSCREEN);
   setBrightness(DEFAULT_VIDEO_BRIGHTNESS);
   setVSync(DEFAULT_VIDEO_VSYNC);
   setShowFps(DEFAULT_VIDEO_SHOWFPS);
   setZoom(DEFAULT_VIDEO_ZOOM);
   setBorderLeft(DEFAULT_VIDEO_BORDERLEFT);
   setBorderTop(DEFAULT_VIDEO_BORDERTOP);
   setBorderRight(DEFAULT_VIDEO_BORDERRIGHT);
   setBorderBottom(DEFAULT_VIDEO_BORDERBOTTOM);
}


void GameSettings::VideoSettings::setWidth(int width)
{
   mWidth = width;
}


int GameSettings::VideoSettings::getWidth() const
{
   return mWidth;
}


void GameSettings::VideoSettings::setHeight(int height)
{
   mHeight = height;
}


int GameSettings::VideoSettings::getHeight() const
{
   return mHeight;
}


void GameSettings::VideoSettings::setResolution(int resolution)
{
   mResolution = resolution;
}

int GameSettings::VideoSettings::getResolution() const
{
   return mResolution;
}


void GameSettings::VideoSettings::setAntialias(int samples)
{
   mAntialias = samples;
}


int GameSettings::VideoSettings::getAntialias() const
{
   return mAntialias;
}


void GameSettings::VideoSettings::setFullscreen(bool fullscreen)
{
   mFullscreen = fullscreen;
}


bool GameSettings::VideoSettings::isFullscreen() const
{
   return mFullscreen;
}


void GameSettings::VideoSettings::toggleFullscreen()
{
   setFullscreen(!isFullscreen());
}


void GameSettings::VideoSettings::setBrightness(float brightness)
{
   mBrightness = brightness;
}


float GameSettings::VideoSettings::getBrightness() const
{
   return mBrightness;
}


void GameSettings::VideoSettings::setVSync(int vsync)
{
   mVSync = vsync;
}


int GameSettings::VideoSettings::getVSync() const
{
   return mVSync;
}


bool GameSettings::VideoSettings::isFpsShown() const
{
   return mShowFps;
}


void GameSettings::VideoSettings::setShowFps(bool value)
{
   mShowFps = value;
}

float GameSettings::VideoSettings::getZoom() const
{
   return mZoom;
}

void GameSettings::VideoSettings::setZoom(float zoom)
{
   mZoom= zoom;
}

int GameSettings::VideoSettings::getBorderLeft() const
{
   return mBorderLeft;
}

void GameSettings::VideoSettings::setBorderLeft(int left)
{
   mBorderLeft= left;
}

int GameSettings::VideoSettings::getBorderTop() const
{
   return mBorderTop;
}

void GameSettings::VideoSettings::setBorderTop(int top)
{
   mBorderTop= top;
}

int GameSettings::VideoSettings::getBorderRight() const
{
   return mBorderRight;
}

void GameSettings::VideoSettings::setBorderRight(int right)
{
   mBorderRight= right;
}

int GameSettings::VideoSettings::getBorderBottom() const
{
   return mBorderBottom;
}

void GameSettings::VideoSettings::setBorderBottom(int bottom)
{
   mBorderBottom= bottom;
}


GameSettings::GameplaySettings::GameplaySettings()
 : mCameraShakeIntensity(1.0f),
   mCameraFollowsPlayer(true)
{
}


void GameSettings::GameplaySettings::serialize()
{
   setValue("gameplay/camerashakeintensity", getCameraShakeIntensity());
   setValue("gameplay/camerafollowsplayer", isCameraFollowingPlayer());
}


void GameSettings::GameplaySettings::deserialize()
{
   setCameraShakeIntensity(
      value(
         "gameplay/camerashakeintensity",
         DEFAULT_GAMEPLAY_CAMERA_SHAKE_INTENSITY
      ).toFloat()
   );

   setCameraFollowsPlayer(
      value(
         "gameplay/camerafollowsplayer",
         DEFAULT_GAMEPLAY_CAMERA_FOLLOWS_PLAYER
      ).toBool()
   );
}


void GameSettings::GameplaySettings::restoreDefaults()
{
   SettingsPrivate::restoreDefaults();

   setCameraShakeIntensity(DEFAULT_GAMEPLAY_CAMERA_SHAKE_INTENSITY);
   setCameraFollowsPlayer(DEFAULT_GAMEPLAY_CAMERA_FOLLOWS_PLAYER);
}


bool GameSettings::GameplaySettings::isCameraFollowingPlayer() const
{
   return mCameraFollowsPlayer;
}


void GameSettings::GameplaySettings::setCameraFollowsPlayer(bool value)
{
   mCameraFollowsPlayer = value;
}


float GameSettings::GameplaySettings::getCameraShakeIntensity() const
{
   return mCameraShakeIntensity;
}


void GameSettings::GameplaySettings::setCameraShakeIntensity(float value)
{
   mCameraShakeIntensity = value;
}


