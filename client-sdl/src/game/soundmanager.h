#pragma once

// SDL3-backed sound effect manager - replaces the original's hand-rolled per-platform mixer
// (client/src/soundsystem/: raw ALSA on Linux, raw Win32 waveOut on Windows) with SDL3's own
// audio API, already a free dependency of this port. Public interface unchanged (same methods
// BombermanClient/GameDrawable already call as fire-and-forget sfx triggers).
//
// Scope: sound EFFECTS only (the 19 short one-shot samples every play*() method below maps to).
// Background music/playlist (client/src/sound/playlist.cpp, 15 mp3 tracks) is a separate, larger
// piece of work - not attempted here - so fadeOut()/restartPlayListAfterFadeOut() (both
// music-only in the original) stay no-ops for now.

// Qt
#include <QObject>

// shared
#include "constants.h"

// SDL
#include <SDL3/SDL_audio.h>

#include <array>

class SoundManager : public QObject
{
   Q_OBJECT

public:
   static SoundManager* getInstance();

   void fadeOut(float fadeOutTime);
   void restartPlayListAfterFadeOut(int delay);

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

   static constexpr int channelCount = 8;

   SDL_AudioDeviceID mDevice = 0;
   std::array<SDL_AudioStream*, channelCount> mChannels{};
   int mNextChannel = 0;
   std::array<Sample, SampleCount> mSamples{};

   static SoundManager* sInstance;
};
