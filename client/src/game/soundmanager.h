#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

// Qt
#include <QObject>
#include <QTime>
#include <QTimer>

// shared
#include "constants.h"

// forward declarations
class Playlist;
class SoundDeviceInterface;
class SamplePlayer;
class StreamPlayer;


class SoundManager : public QObject
{
   Q_OBJECT

public:

   //! constructor
   SoundManager(bool captureMode = false);

   //! destructor
   virtual ~SoundManager();

   //! static instance getter
   static SoundManager* getInstance();

   //! initialize soundmanager
   virtual void initialize();

   //! getter for sample player
   SamplePlayer* getSamplePlayer() const;
   
   //! getter for stream player
   StreamPlayer* getStreamPlayer() const;


   // audio

   //! getter for music volume
   virtual float getVolumeMusic() const;

   //! getter for sfx volume
   virtual float getVolumeSfx() const;

   //! start the playlist
   void startPlaylist();

   //! fade out
   void fadeOut(float fadeOutTime);

   //! restart playlist after fadeout
   void restartPlayListAfterFadeOut(int delay);

   //! check if music is muted
   bool isMusicMuted() const;

   //! check if sfx are muted
   bool isSfxMuted() const;


signals:
   
   // music

   //! playlist change
   void musicPlaying(
      const QString& artist,
      const QString& album,
      const QString& track
   );


   // fade out

   //! fade out finished
   void fadeOutFinished();


public slots:

   // connect playlist

   //! play next track
   virtual void playNextTrack();

   //! play previous track
   virtual void playPreviousTrack();

   //! reset stream player volume
   virtual void resetStreamPlayerVolume();

   //! enable or disable shuffle
   virtual void setShuffleEnabled(bool shuffle);


   // music and sfx

   //! setter for music volume
   virtual void setVolumeMusic(float volume);

   //! setter for sfx volume
   virtual void setVolumeSfx(float volume);

   //! set default music volume
   virtual void setVolumeMusicToDefault();

   //! set default sfx volume
   virtual void setVolumeSfxToDefault();

   //! toggle music mute
   virtual void toggleMuteMusic();

   //! toggle sfx mute
   virtual void toggleMuteSfx();


   // play sounds

   //! play killed sound
   void playSoundKilled();

   //! play bomb sound
   void playSoundBomb();

   //! play start sound
   void playSoundStart();

   //! play extra sound
   void playSoundExtra();

   //! play time left sample
   void playSoundTime(int time);

   //! play a tick
   void playSoundTick();

   //! play mouse over sample
   void playSoundMouseOver(const QString&, const QString&);

   //! play mouse click
   void playSoundMouseClick(const QString & page);

   //! play "player joined" sound
   void playSoundPlayerJoined();

   //! play "player left" sound
   void playSoundPlayerLeft();

   //! play sound draw
   void playSoundGameDraw();

   //! play sound win
   void playSoundGameWin();

   //! play error sound
   void playSoundError();

   //! play info sound
   void playSoundInfo();

   //! play sound kick
   void playSoundKick();

   //! play sound hurry up
   void playSoundHurryUp();

   //! play sound message sent
   void playSoundMessageSent();

   //! play sound message received
   void playSoundMessageReceived();

   //! play sound bomb bounce
   void playSoundBombBounce();

   //! play sound shake box
   void playSoundBoxShake();

   //! play sound extra revealed
   void playSoundExtraRevealed();

   //! play sound extra mushroom
   void playSoundExtraMushroom();

   //! play sound extra invisible
   void playSoundExtraInvisible();

   //! play sound extra invulnerable
   void playSoundExtraInvulnerable();

   //! play skull sound
   void playSkullSound(Constants::SkullType skullType);


protected slots:

   //! update fade out
   void updateFadeOut();


protected:

   // audio

   //! init samples
   void initializeSamples();

   //! init playlist
   void initializePlaylist();

   //! initialize volume
   void initializeAudio();

   //! mixer
   SoundDeviceInterface* mSoundDevice;
   SamplePlayer* mSamplePlayer;
   StreamPlayer* mStreamPlayer;

   //! playlist
   Playlist* mPlaylist;

   int mSoundBomb;
   int mSoundExtra;
   int mSoundKilled;
   int mSoundStart;
   int mSoundDraw;
   int mSoundWin;
   int mSoundCountdown[3];
   int mSoundMenusMouseOver;
   int mSoundMenusMouseClick;
   int mSoundLoungePlayerJoined;
   int mSoundLoungePlayerLeft;
   int mSoundError;
   int mSoundInfo;
   int mSoundKick;
   int mSoundHurryUp;
   int mSoundMessageSent;
   int mSoundMessageReceived;
   int mSoundBombBounce;
   int mSoundBoxShake;
   int mSoundExtraRevealed;
   int mSoundExtraMushroom;
   int mSoundExtraInvisible;
   int mSoundExtraInvulnerable;

   //! do not play page first change event
   bool mSoundMenusMouseClickInitialized;

   //! getter for client singleton
   static SoundManager* mInstance;


   // sound fading

   //! fade length
   float mFadeDuration;

   //! original stream volume
   float mFadeStreamVolume;

   //! fade timer
   QTimer mFadeTimer;

   //! fade time
   QTime mFadeTime;
};

#endif // SOUNDMANAGER_H
