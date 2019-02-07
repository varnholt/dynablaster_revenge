// header
#include "soundmanager.h"

// Qt
#include <QDir>

// game
#include "gamesettings.h"
#include "tools/filestream.h"
#include "math/vector.h"

// mixer
#include "../soundsystem/sounddeviceinterface.h"
#include "../soundsystem/sounddevice.h"
#include "../soundsystem/dummysounddevice.h"
#include "../soundsystem/streamplayer.h"
#include "../soundsystem/sampleplayer.h"
#include "../sound/playlist.h"

// audio
#define SAMPLE_GAME_BOMB                 "data/sfx/bomb.wav"
#define SAMPLE_GAME_EXTRA                "data/sfx/extra.wav"
#define SAMPLE_GAME_KILLED               "data/sfx/killed.wav"
#define SAMPLE_GAME_START                "data/sfx/start.wav"
#define SAMPLE_GAME_DRAW                 "data/sfx/draw.wav"
#define SAMPLE_GAME_WIN                  "data/sfx/win.wav"
#define SAMPLE_GAME_ERROR                "data/sfx/error.wav"
#define SAMPLE_GAME_INFO                 "data/sfx/info.wav"
#define SAMPLE_GAME_BOMB_KICK            "data/sfx/kick.wav"
#define SAMPLE_GAME_HURRY_UP             "data/sfx/hurry_up.wav"
#define SAMPLE_GAME_BOMB_PLACED          ""
#define SAMPLE_GAME_BOMB_BOUNCE          "data/sfx/bounce.wav"
#define SAMPLE_GAME_BOX_SHAKE            "data/sfx/shake.wav"
#define SAMPLE_GAME_EXTRA_REVEALED       "data/sfx/extra_revealed.wav"
#define SAMPLE_GAME_EXTRA_MUSHROOM       "data/sfx/extra_mushroom.wav"
#define SAMPLE_GAME_EXTRA_INVISIBLE      "data/sfx/extra_invisible.wav"
#define SAMPLE_GAME_EXTRA_INVULNERABLE   "data/sfx/extra_invulnerable.wav"
#define SAMPLE_GAME_PLAYER_WALK          "data/sfx/walking.wav"
#define SAMPLE_GAME_FINISHED             "data/sfx/finished.wav"
#define SAMPLE_GAME_MESSAGE_SENT         "data/sfx/message_sent.wav"
#define SAMPLE_GAME_MESSAGE_RECEIVED     "data/sfx/message_received.wav"

#define SAMPLE_MENUS_MOUSE_OVER           "data/sfx/mouse_over.wav"
#define SAMPLE_MENUS_MOUSE_CLICK          "data/sfx/mouse_click.wav"
#define SAMPLE_MENUS_MOUSE_RELEASE        ""

#define SAMPLE_MENUS_LOUNGE_COUNTDOWN_1   "data/sfx/countdown_1.wav"
#define SAMPLE_MENUS_LOUNGE_COUNTDOWN_2   "data/sfx/countdown_2.wav"
#define SAMPLE_MENUS_LOUNGE_COUNTDOWN_3   "data/sfx/countdown_3.wav"
#define SAMPLE_MENUS_LOUNGE_PLAYER_JOINED "data/sfx/player_joined.wav"
#define SAMPLE_MENUS_LOUNGE_PLAYER_LEFT   "data/sfx/player_left.wav"

// static variables
SoundManager* SoundManager::mInstance = 0;


//----------------------------------------------------------------------------
/*!
   \param parent parent object
*/
SoundManager::SoundManager(bool captureMode) :
   QObject(0),
   mSoundDevice(0),
   mSamplePlayer(0),
   mStreamPlayer(0),
   mPlaylist(0),
   mSoundBomb(-1),
   mSoundExtra(-1),
   mSoundKilled(-1),
   mSoundStart(-1),
   mSoundDraw(-1),
   mSoundWin(-1),
   mSoundMenusMouseOver(-1),
   mSoundMenusMouseClick(-1),
   mSoundLoungePlayerJoined(-1),
   mSoundLoungePlayerLeft(-1),
   mSoundError(-1),
   mSoundInfo(-1),
   mSoundKick(-1),
   mSoundHurryUp(-1),
   mSoundMessageSent(-1),
   mSoundMessageReceived(-1),
   mSoundBombBounce(-1),
   mSoundBoxShake(-1),
   mSoundExtraRevealed(-1),
   mSoundExtraMushroom(-1),
   mSoundExtraInvisible(-1),
   mSoundExtraInvulnerable(-1),
   mSoundMenusMouseClickInitialized(false)
{
   mSoundCountdown[0]=0;
   mSoundCountdown[1]=0;
   mSoundCountdown[2]=0;

   mInstance = this;

   bool music =
      GameSettings::getInstance()->getDevelopmentSettings()->isMusicEnabled();

   // init sound device
   if (captureMode)
   {
      FILE *output= fopen("f:\\capture.raw", "wb");
      mSoundDevice = new DummySoundDevice( output );
   }
   else
      mSoundDevice = new SoundDevice();


/*
   FILE *file= fopen("f:\\test16.raw", "wb");
   mSoundDevice->setCaptureFile(file);
*/

   if (!mSoundDevice->init(44100, 256, 16))
   {
      qWarning("SoundManager::SoundManager: sound device initialized failed!");
   }

   // init players
   mSamplePlayer = new SamplePlayer();
   mSoundDevice->addAudioSource(mSamplePlayer);

   mStreamPlayer= new StreamPlayer();
   mSoundDevice->addAudioSource(mStreamPlayer);

   // init playlist
   if (music)
      mPlaylist = new Playlist();

   // connect fade timer
   connect(
      &mFadeTimer,
      SIGNAL(timeout()),
      this,
      SLOT(updateFadeOut())
   );
}


//----------------------------------------------------------------------------
/*!
*/
SoundManager::~SoundManager()
{
   delete mSamplePlayer;
   delete mStreamPlayer;
   delete mPlaylist;

   mSoundDevice->stop();
}


//----------------------------------------------------------------------------
/*!
   \return singleton instance
*/
SoundManager *SoundManager::getInstance()
{
   if (!mInstance)
   {
      mInstance = new SoundManager();
   }

   return mInstance;
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::initialize()
{
   initializeSamples();
   initializePlaylist();

   // playlist
   connect(
      mPlaylist,
      SIGNAL(playing(QString,QString,QString)),
      this,
      SIGNAL(musicPlaying(QString,QString,QString))
   );

   mSoundDevice->start();

   initializeAudio();
}


//----------------------------------------------------------------------------
/*!
   return ptr to sample player
*/
SamplePlayer *SoundManager::getSamplePlayer() const
{
   return mSamplePlayer;
}



//----------------------------------------------------------------------------
/*!
   \return ptr to stream player
*/
StreamPlayer *SoundManager::getStreamPlayer() const
{
   return mStreamPlayer;
}


//----------------------------------------------------------------------------
/*!
   \return music volume
*/
float SoundManager::getVolumeMusic() const
{
   return mStreamPlayer->getVolume();
}


//----------------------------------------------------------------------------
/*!
   \return sfx volume
*/
float SoundManager::getVolumeSfx() const
{
   return mSamplePlayer->getVolume();
}


//-----------------------------------------------------------------------------
/*!
*/
void SoundManager::initializeSamples()
{
   mSoundBomb = mSamplePlayer->addSample(SAMPLE_GAME_BOMB);
   mSoundExtra = mSamplePlayer->addSample(SAMPLE_GAME_EXTRA);
   mSoundKilled = mSamplePlayer->addSample(SAMPLE_GAME_KILLED);
   mSoundStart= mSamplePlayer->addSample(SAMPLE_GAME_START);
   mSoundDraw = mSamplePlayer->addSample(SAMPLE_GAME_DRAW);
   mSoundWin = mSamplePlayer->addSample(SAMPLE_GAME_WIN);
   mSoundCountdown[0] = mSamplePlayer->addSample(SAMPLE_MENUS_LOUNGE_COUNTDOWN_1);
   mSoundCountdown[1] = mSamplePlayer->addSample(SAMPLE_MENUS_LOUNGE_COUNTDOWN_2);
   mSoundCountdown[2] = mSamplePlayer->addSample(SAMPLE_MENUS_LOUNGE_COUNTDOWN_3);
   mSoundMenusMouseOver = mSamplePlayer->addSample(SAMPLE_MENUS_MOUSE_OVER);
   mSoundMenusMouseClick = mSamplePlayer->addSample(SAMPLE_MENUS_MOUSE_CLICK);
   mSoundLoungePlayerJoined = mSamplePlayer->addSample(SAMPLE_MENUS_LOUNGE_PLAYER_JOINED);
   mSoundLoungePlayerLeft = mSamplePlayer->addSample(SAMPLE_MENUS_LOUNGE_PLAYER_LEFT);
   mSoundError = mSamplePlayer->addSample(SAMPLE_GAME_ERROR);
   mSoundInfo = mSamplePlayer->addSample(SAMPLE_GAME_INFO);
   mSoundKick = mSamplePlayer->addSample(SAMPLE_GAME_BOMB_KICK);
   mSoundHurryUp = mSamplePlayer->addSample(SAMPLE_GAME_HURRY_UP);
   mSoundMessageSent = mSamplePlayer->addSample(SAMPLE_GAME_MESSAGE_SENT);
   mSoundMessageReceived = mSamplePlayer->addSample(SAMPLE_GAME_MESSAGE_RECEIVED);
   mSoundBombBounce = mSamplePlayer->addSample(SAMPLE_GAME_BOMB_BOUNCE);
   mSoundBoxShake = mSamplePlayer->addSample(SAMPLE_GAME_BOX_SHAKE);
   mSoundExtraRevealed = mSamplePlayer->addSample(SAMPLE_GAME_EXTRA_REVEALED);
   mSoundExtraMushroom = mSamplePlayer->addSample(SAMPLE_GAME_EXTRA_MUSHROOM);
   mSoundExtraInvisible = mSamplePlayer->addSample(SAMPLE_GAME_EXTRA_INVISIBLE);
   mSoundExtraInvulnerable = mSamplePlayer->addSample(SAMPLE_GAME_EXTRA_INVULNERABLE);
}


//-----------------------------------------------------------------------------
/*!
*/
void SoundManager::initializePlaylist()
{
   if (mPlaylist)
   {
      mPlaylist->setMixer(mStreamPlayer);

      QDir dir("data/music");
      QStringList filters;
      filters << "*.mp3";
      dir.setNameFilters(filters);

      FileStream::addPath("data/music");
      mPlaylist->setPlaylist(dir.entryList(filters));
   }
   else
   {
      qWarning("BombermanClient::initPlaylist: playlist not initialized");
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void SoundManager::startPlaylist()
{
   if (mPlaylist)
      mPlaylist->play();
}


//-----------------------------------------------------------------------------
/*!
   \param fade out time
*/
void SoundManager::fadeOut(float fadeOutTime)
{
   if (!mFadeTimer.isActive())
   {
      mFadeStreamVolume = mStreamPlayer->getVolume();
      mFadeDuration = fadeOutTime;
      mFadeTime.restart();
      mFadeTimer.start(50);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param delay delay to wait
*/
void SoundManager::restartPlayListAfterFadeOut(int delay)
{
   mStreamPlayer->stop();

   QTimer::singleShot(delay, this, SLOT(playNextTrack()));

   connect(
      mPlaylist,
      SIGNAL(playing(QString,QString,QString)),
      this,
      SLOT(resetStreamPlayerVolume())
   );
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if music is muted
*/
bool SoundManager::isMusicMuted() const
{
   return mStreamPlayer->isMuted();
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if sfx are muted
*/
bool SoundManager::isSfxMuted() const
{
   return mSamplePlayer->isMuted();
}


//-----------------------------------------------------------------------------
/*!
*/
void SoundManager::resetStreamPlayerVolume()
{
   disconnect(
      mPlaylist,
      SIGNAL(playing(QString,QString,QString)),
      this,
      SLOT(resetStreamPlayerVolume())
   );

   mStreamPlayer->setVolume(
      GameSettings::getInstance()->getAudioSettings()->getVolumeMusic()
   );
}


//-----------------------------------------------------------------------------
/*!
   \param shuffle shuffle enabled or not
*/
void SoundManager::setShuffleEnabled(bool shuffle)
{
   if (mPlaylist)
      mPlaylist->setShuffleEnabled(shuffle);
}


//-----------------------------------------------------------------------------
/*!
*/
void SoundManager::updateFadeOut()
{
   float factor = 0.0;

   factor = qMax((1.0f - (mFadeTime.elapsed() / mFadeDuration)), 0.0f);

   mStreamPlayer->setVolume(factor * mFadeStreamVolume);

   if (factor == 0.0f)
   {
      mFadeTimer.stop();
      emit fadeOutFinished();
   }
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::initializeAudio()
{
   setVolumeMusic(
      GameSettings::getInstance()->getAudioSettings()->getVolumeMusic()
   );

   setVolumeSfx(
      GameSettings::getInstance()->getAudioSettings()->getVolumeSfx()
   );
}


//----------------------------------------------------------------------------
/*!
   \param volume volume to set
*/
void SoundManager::setVolumeMusic(float volume)
{
   mStreamPlayer->setVolume(volume);
}


//----------------------------------------------------------------------------
/*!
   \param volume volume to set
*/
void SoundManager::setVolumeSfx(float volume)
{
   mSamplePlayer->setVolume(volume);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::setVolumeMusicToDefault()
{
   setVolumeMusic(
      GameSettings::getInstance()->getAudioSettings()->getVolumeMusicDefault()
   );
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::setVolumeSfxToDefault()
{
   setVolumeSfx(
      GameSettings::getInstance()->getAudioSettings()->getVolumeSfxDefault()
   );
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::toggleMuteMusic()
{
   mStreamPlayer->toggleMute();
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::toggleMuteSfx()
{
   mSamplePlayer->toggleMute();
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundKilled()
{
   mSamplePlayer->play(mSoundKilled, Vector(0.0f, 0.0f, 0.0f));
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundBomb()
{
   mSamplePlayer->play( mSoundBomb, Vector(0.0f, 0.0f, 0.0f) );
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundStart()
{
   mSamplePlayer->play( mSoundStart, Vector(0.0f, 0.0f, 0.0f) );
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundExtra()
{
   mSamplePlayer->play( mSoundExtra, Vector(0.0f, 0.0f, 0.0f) );
}


//----------------------------------------------------------------------------
/*!
   \param time time to play
*/
void SoundManager::playSoundTime(int time)
{
   if (time >= 1 && time <= 3)
   {
      mSamplePlayer->play( mSoundCountdown[time-1], Vector(0.0f, 0.0f, 0.0f) );
   }
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundTick()
{
   if (mSamplePlayer->getChannelCount() == 0)
      mSamplePlayer->play( mSoundMenusMouseOver, Vector(0.0f, 0.0f, 0.0f) );
}



//----------------------------------------------------------------------------
/*!
   \param page current menu page
   \param item item under mouse cursor
*/
void SoundManager::playSoundMouseOver(const QString& /*page*/, const QString& item)
{
   if (item.startsWith("button"))
      mSamplePlayer->play( mSoundMenusMouseOver, Vector(0.0f, 0.0f, 0.0f) );
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundMouseClick(const QString& /*page*/)
{
   // qDebug("PAGE: %s", qPrintable(page));
   if (mSoundMenusMouseClickInitialized)
   {
      mSamplePlayer->play(mSoundMenusMouseClick);
   }
   else
   {
      mSoundMenusMouseClickInitialized = true;
   }
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundPlayerJoined()
{
   mSamplePlayer->play( mSoundLoungePlayerJoined, Vector(0.0f, 0.0f, 0.0f) );
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundPlayerLeft()
{
   mSamplePlayer->play( mSoundLoungePlayerLeft, Vector(0.0f, 0.0f, 0.0f) );
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundGameDraw()
{
   mSamplePlayer->play(mSoundDraw);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundGameWin()
{
   mSamplePlayer->play(mSoundWin);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundError()
{
   mSamplePlayer->play(mSoundError);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundInfo()
{
   mSamplePlayer->play(mSoundInfo);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundKick()
{
   mSamplePlayer->play(mSoundKick);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundHurryUp()
{
   mSamplePlayer->play(mSoundHurryUp);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundMessageSent()
{
   mSamplePlayer->play(mSoundMessageSent);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundMessageReceived()
{
   mSamplePlayer->play(mSoundMessageReceived);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundBombBounce()
{
   mSamplePlayer->play(mSoundBombBounce);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundBoxShake()
{
   mSamplePlayer->play(mSoundBoxShake);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundExtraRevealed()
{
   mSamplePlayer->play(mSoundExtraRevealed);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundExtraMushroom()
{
   mSamplePlayer->play(mSoundExtraMushroom);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundExtraInvisible()
{
   mSamplePlayer->play(mSoundExtraInvisible);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playSoundExtraInvulnerable()
{
   mSamplePlayer->play(mSoundExtraInvulnerable);
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playNextTrack()
{
   if (mPlaylist)
      mPlaylist->playNext();
}


//----------------------------------------------------------------------------
/*!
*/
void SoundManager::playPreviousTrack()
{
   if (mPlaylist)
      mPlaylist->playPrevious();
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void SoundManager::playSkullSound(Constants::SkullType skullType)
{
   switch (skullType)
   {
      case Constants::SkullAutofire:
         break;

      case Constants::SkullKeyboardInvert:
         break;

      case Constants::SkullMushroom:
         playSoundExtraMushroom();
         break;

      case Constants::SkullInvisible:
         playSoundExtraInvisible();
         break;

      case Constants::SkullInvincible:
         playSoundExtraInvulnerable();
         break;

      case Constants::SkullNoBomb:
         break;

      default:
         break;
   }
}

