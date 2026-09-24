#include "soundmanager.h"

#include <SDL3/SDL.h>

SoundManager* SoundManager::sInstance = nullptr;

SoundManager::SoundManager() : QObject(nullptr)
{
   sInstance = this;

   if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
   {
      qWarning("SoundManager: SDL_InitSubSystem(SDL_INIT_AUDIO) failed: %s", SDL_GetError());
      return;
   }

   mDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
   if (mDevice == 0)
   {
      qWarning("SoundManager: SDL_OpenAudioDevice failed: %s", SDL_GetError());
      return;
   }

   for (auto& channel : mChannels)
      channel = SDL_CreateAudioStream(nullptr, nullptr);

   SDL_BindAudioStreams(mDevice, mChannels.data(), static_cast<int>(mChannels.size()));
   SDL_ResumeAudioDevice(mDevice);

   initializeSamples();
}

SoundManager::~SoundManager()
{
   for (auto& channel : mChannels)
   {
      if (channel)
      {
         SDL_UnbindAudioStream(channel);
         SDL_DestroyAudioStream(channel);
      }
   }

   for (auto& sample : mSamples)
   {
      if (sample.buffer)
         SDL_free(sample.buffer);
   }

   if (mDevice)
      SDL_CloseAudioDevice(mDevice);
}

SoundManager* SoundManager::getInstance()
{
   if (!sInstance)
      sInstance = new SoundManager();

   return sInstance;
}

void SoundManager::initializeSamples()
{
   loadSample(SampleBomb, "data/sfx/bomb.wav");
   loadSample(SampleExtra, "data/sfx/extra.wav");
   loadSample(SampleKilled, "data/sfx/killed.wav");
   loadSample(SampleStart, "data/sfx/start.wav");
   loadSample(SampleCountdown1, "data/sfx/countdown_1.wav");
   loadSample(SampleCountdown2, "data/sfx/countdown_2.wav");
   loadSample(SampleCountdown3, "data/sfx/countdown_3.wav");
   loadSample(SamplePlayerJoined, "data/sfx/player_joined.wav");
   loadSample(SamplePlayerLeft, "data/sfx/player_left.wav");
   loadSample(SampleKick, "data/sfx/kick.wav");
   loadSample(SampleHurryUp, "data/sfx/hurry_up.wav");
   loadSample(SampleMessageSent, "data/sfx/message_sent.wav");
   loadSample(SampleMessageReceived, "data/sfx/message_received.wav");
   loadSample(SampleBombBounce, "data/sfx/bounce.wav");
   loadSample(SampleBoxShake, "data/sfx/shake.wav");
   loadSample(SampleExtraRevealed, "data/sfx/extra_revealed.wav");
   loadSample(SampleExtraMushroom, "data/sfx/extra_mushroom.wav");
   loadSample(SampleExtraInvisible, "data/sfx/extra_invisible.wav");
   loadSample(SampleExtraInvulnerable, "data/sfx/extra_invulnerable.wav");
}

void SoundManager::loadSample(SampleId id, const char* filename)
{
   Sample& sample = mSamples[id];
   Uint8* buffer = nullptr;
   Uint32 length = 0;

   if (!SDL_LoadWAV(filename, &sample.spec, &buffer, &length))
   {
      qWarning("SoundManager: failed to load %s: %s", filename, SDL_GetError());
      return;
   }

   sample.buffer = buffer;
   sample.length = length;
}

void SoundManager::play(SampleId id)
{
   if (mDevice == 0)
      return;

   const Sample& sample = mSamples[id];
   if (!sample.buffer)
      return;

   SDL_AudioStream* channel = mChannels[mNextChannel];
   mNextChannel = (mNextChannel + 1) % channelCount;

   SDL_ClearAudioStream(channel);
   SDL_SetAudioStreamFormat(channel, &sample.spec, nullptr);
   SDL_PutAudioStreamData(channel, sample.buffer, static_cast<int>(sample.length));
}

void SoundManager::fadeOut(float /*fadeOutTime*/)
{
   // music/playlist system not ported yet - see class comment.
}

void SoundManager::restartPlayListAfterFadeOut(int /*delay*/)
{
   // music/playlist system not ported yet - see class comment.
}

void SoundManager::playSoundKilled()
{
   play(SampleKilled);
}

void SoundManager::playSoundBomb()
{
   play(SampleBomb);
}

void SoundManager::playSoundStart()
{
   play(SampleStart);
}

void SoundManager::playSoundExtra()
{
   play(SampleExtra);
}

void SoundManager::playSoundTime(int time)
{
   if (time == 1)
      play(SampleCountdown1);
   else if (time == 2)
      play(SampleCountdown2);
   else if (time == 3)
      play(SampleCountdown3);
}

void SoundManager::playSoundPlayerJoined()
{
   play(SamplePlayerJoined);
}

void SoundManager::playSoundPlayerLeft()
{
   play(SamplePlayerLeft);
}

void SoundManager::playSoundKick()
{
   play(SampleKick);
}

void SoundManager::playSoundHurryUp()
{
   play(SampleHurryUp);
}

void SoundManager::playSoundMessageSent()
{
   play(SampleMessageSent);
}

void SoundManager::playSoundMessageReceived()
{
   play(SampleMessageReceived);
}

void SoundManager::playSoundBombBounce()
{
   play(SampleBombBounce);
}

void SoundManager::playSoundBoxShake()
{
   play(SampleBoxShake);
}

void SoundManager::playSoundExtraRevealed()
{
   play(SampleExtraRevealed);
}

void SoundManager::playSkullSound(Constants::SkullType skullType)
{
   switch (skullType)
   {
      case Constants::SkullMushroom:
         play(SampleExtraMushroom);
         break;

      case Constants::SkullInvisible:
         play(SampleExtraInvisible);
         break;

      case Constants::SkullInvincible:
         play(SampleExtraInvulnerable);
         break;

      default:
         break;
   }
}
