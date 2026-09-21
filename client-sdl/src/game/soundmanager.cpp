#include "soundmanager.h"

SoundManager* SoundManager::sInstance = nullptr;

SoundManager::SoundManager()
   : QObject()
{
}

SoundManager* SoundManager::getInstance()
{
   if (!sInstance)
      sInstance = new SoundManager();

   return sInstance;
}

void SoundManager::fadeOut(float) {}
void SoundManager::restartPlayListAfterFadeOut(int) {}
void SoundManager::playSoundKilled() {}
void SoundManager::playSoundBomb() {}
void SoundManager::playSoundStart() {}
void SoundManager::playSoundExtra() {}
void SoundManager::playSoundTime(int) {}
void SoundManager::playSoundPlayerJoined() {}
void SoundManager::playSoundPlayerLeft() {}
void SoundManager::playSoundKick() {}
void SoundManager::playSoundHurryUp() {}
void SoundManager::playSoundMessageSent() {}
void SoundManager::playSoundMessageReceived() {}
void SoundManager::playSoundBombBounce() {}
void SoundManager::playSoundBoxShake() {}
void SoundManager::playSoundExtraRevealed() {}
void SoundManager::playSkullSound(Constants::SkullType) {}
