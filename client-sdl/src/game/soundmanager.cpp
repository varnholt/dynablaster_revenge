#include "soundmanager.h"

#include "gamesettings.h"

#include <SDL3/SDL.h>

#define MINIMP3_IMPLEMENTATION
#include "minimp3_ex.h"

#include <algorithm>

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

   mMusicStream = SDL_CreateAudioStream(nullptr, nullptr);
   SDL_BindAudioStream(mDevice, mMusicStream);

   SDL_ResumeAudioDevice(mDevice);

   initializeSamples();

   mVolumeMusic = GameSettings::getInstance()->getAudioSettings()->getVolumeMusic();
   mVolumeSfx = GameSettings::getInstance()->getAudioSettings()->getVolumeSfx();
   SDL_SetAudioStreamGain(mMusicStream, mVolumeMusic);

   connect(&mMusicTimer, &QTimer::timeout, this, &SoundManager::updateMusic);
   mMusicTimer.start(50);
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

   if (mMusicStream)
   {
      SDL_UnbindAudioStream(mMusicStream);
      SDL_DestroyAudioStream(mMusicStream);
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
   loadSample(SampleMouseOver, "data/sfx/mouse_over.wav");
   loadSample(SampleMouseClick, "data/sfx/mouse_click.wav");
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
   SDL_SetAudioStreamGain(channel, mVolumeSfx);
   SDL_SetAudioStreamFormat(channel, &sample.spec, nullptr);
   SDL_PutAudioStreamData(channel, sample.buffer, static_cast<int>(sample.length));
}

void SoundManager::fadeOut(float fadeOutTime)
{
   if (mFading || !mMusicStream)
      return;

   mFading = true;
   mFadeStartVolume = SDL_GetAudioStreamGain(mMusicStream);
   mFadeDurationMs = fadeOutTime;
   mFadeElapsedMs = 0.0f;
}

void SoundManager::restartPlayListAfterFadeOut(int delay)
{
   if (mMusicStream)
      SDL_ClearAudioStream(mMusicStream);

   QTimer::singleShot(
      delay,
      this,
      [this]()
      {
         SDL_SetAudioStreamGain(mMusicStream, mVolumeMusic);
         startPlaylist();
      }
   );
}

float SoundManager::getVolumeMusic() const
{
   return mVolumeMusic;
}

float SoundManager::getVolumeSfx() const
{
   return mVolumeSfx;
}

void SoundManager::setVolumeMusic(float volume)
{
   mVolumeMusic = volume;

   if (mMusicStream && !mFading)
      SDL_SetAudioStreamGain(mMusicStream, mVolumeMusic);
}

void SoundManager::setVolumeSfx(float volume)
{
   mVolumeSfx = volume;
}

void SoundManager::startPlaylist()
{
   if (mPlaylist.empty())
   {
      for (const auto& entry : std::filesystem::directory_iterator("data/music"))
      {
         if (entry.path().extension() == ".mp3")
            mPlaylist.push_back(entry.path());
      }

      std::sort(mPlaylist.begin(), mPlaylist.end());
   }

   if (mPlaylist.empty())
   {
      qWarning("SoundManager: no music found in data/music");
      return;
   }

   mTrackIndex = 0;
   playTrack(mTrackIndex);
}

void SoundManager::playTrack(std::size_t index)
{
   if (!mMusicStream || mPlaylist.empty())
      return;

   const std::string path = mPlaylist[index % mPlaylist.size()].string();

   mp3dec_t decoder;
   mp3dec_file_info_t info{};
   if (mp3dec_load(&decoder, path.c_str(), &info, nullptr, nullptr) != 0 || !info.buffer)
   {
      qWarning("SoundManager: failed to decode %s", path.c_str());
      return;
   }

   SDL_AudioSpec spec{};
   spec.format = SDL_AUDIO_S16;
   spec.channels = info.channels;
   spec.freq = info.hz;

   SDL_ClearAudioStream(mMusicStream);
   SDL_SetAudioStreamFormat(mMusicStream, &spec, nullptr);
   SDL_PutAudioStreamData(mMusicStream, info.buffer, static_cast<int>(info.samples * sizeof(mp3d_sample_t)));

   free(info.buffer);
}

void SoundManager::updateMusic()
{
   if (!mMusicStream)
      return;

   if (mFading)
   {
      mFadeElapsedMs += 50.0f;
      const float factor = (std::max)(1.0f - mFadeElapsedMs / mFadeDurationMs, 0.0f);
      SDL_SetAudioStreamGain(mMusicStream, factor * mFadeStartVolume);

      if (factor <= 0.0f)
      {
         SDL_ClearAudioStream(mMusicStream);
         mFading = false;
      }

      return;
   }

   if (!mPlaylist.empty() && SDL_GetAudioStreamQueued(mMusicStream) == 0)
   {
      mTrackIndex = (mTrackIndex + 1) % mPlaylist.size();
      playTrack(mTrackIndex);
   }
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

void SoundManager::playSoundMouseOver(const QString& /*page*/, const QString& item)
{
   if (item.startsWith("button"))
      play(SampleMouseOver);
}

void SoundManager::playSoundMouseClick(const QString& /*page*/)
{
   if (mMouseClickInitialized)
      play(SampleMouseClick);
   else
      mMouseClickInitialized = true;
}

void SoundManager::playSoundTick()
{
   play(SampleMouseOver);
}
