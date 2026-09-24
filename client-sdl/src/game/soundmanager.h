#pragma once

// SDL3-backed sound manager - replaces the original's hand-rolled per-platform mixer.
// Music decoding uses minimp3 (see CMakeLists.txt).

// Qt
#include <QObject>
#include <QString>
#include <QTimer>

// shared
#include "constants.h"

// SDL
#include <SDL3/SDL_audio.h>

#include <array>
#include <filesystem>
#include <vector>

class SoundManager : public QObject
{
   Q_OBJECT

public:
   static SoundManager* getInstance();

   void fadeOut(float fadeOutTime);
   void restartPlayListAfterFadeOut(int delay);

   void startPlaylist();

   float getVolumeMusic() const;
   float getVolumeSfx() const;
   void setVolumeMusic(float volume);
   void setVolumeSfx(float volume);

public slots:

   void playSoundKilled();
   void playSoundBomb();
   void playSoundStart();
   void playSoundExtra();
   void playSoundTime(int time);
   void playSoundPlayerJoined();
   void playSoundPlayerLeft();
   void playSoundKick();
   void playSoundHurryUp();
   void playSoundMessageSent();
   void playSoundMessageReceived();
   void playSoundBombBounce();
   void playSoundBoxShake();
   void playSoundExtraRevealed();
   void playSkullSound(Constants::SkullType skullType);

   void playSoundMouseOver(const QString& page, const QString& item);
   void playSoundMouseClick(const QString& page);
   void playSoundTick();

private slots:
   // ticks on mMusicTimer; auto-advances finished tracks and drives the fade-out ramp.
   void updateMusic();

protected:
   SoundManager();
   ~SoundManager() override;

   enum SampleId
   {
      SampleBomb,
      SampleExtra,
      SampleKilled,
      SampleStart,
      SampleCountdown1,
      SampleCountdown2,
      SampleCountdown3,
      SamplePlayerJoined,
      SamplePlayerLeft,
      SampleKick,
      SampleHurryUp,
      SampleMessageSent,
      SampleMessageReceived,
      SampleBombBounce,
      SampleBoxShake,
      SampleExtraRevealed,
      SampleExtraMushroom,
      SampleExtraInvisible,
      SampleExtraInvulnerable,
      SampleMouseOver,
      SampleMouseClick,
      SampleCount
   };

   struct Sample
   {
      Uint8* buffer = nullptr;
      Uint32 length = 0;
      SDL_AudioSpec spec{};
   };

   void initializeSamples();
   void loadSample(SampleId id, const char* filename);

   // picks the next of a small round-robin pool of mixed-together channels (matches the
   // original's own fixed-channel-count SamplePlayer, referenced by its getChannelCount()) and
   // feeds it this sample's PCM data - cutting off whatever that channel was still playing, same
   // trade-off any simple fixed-channel sfx mixer makes.
   void play(SampleId id);

   // decodes the whole track up front via minimp3 and queues it in one go.
   void playTrack(std::size_t index);

   static constexpr int channelCount = 8;

   SDL_AudioDeviceID mDevice = 0;
   std::array<SDL_AudioStream*, channelCount> mChannels{};
   int mNextChannel = 0;
   std::array<Sample, SampleCount> mSamples{};

   // skips the click sound on the very first pageChanged (initial page load, not a real click).
   bool mMouseClickInitialized = false;

   float mVolumeMusic = 1.0f;
   float mVolumeSfx = 1.0f;

   SDL_AudioStream* mMusicStream = nullptr;
   std::vector<std::filesystem::path> mPlaylist;
   std::size_t mTrackIndex = 0;
   QTimer mMusicTimer;

   bool mFading = false;
   float mFadeStartVolume = 1.0f;
   float mFadeDurationMs = 1000.0f;
   float mFadeElapsedMs = 0.0f;

   static SoundManager* sInstance;
};
