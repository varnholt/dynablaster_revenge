#include "playbackpacketfilter.h"


PlaybackPacketFilter::PlaybackPacketFilter()
 : mGameId(0),
   mRoundNumber(0),
   mEnabled(false),
   mRunning(false),
   mRoundCounter(0)
{
}


void PlaybackPacketFilter::setGameId(int id)
{
   mGameId = id;
}


int PlaybackPacketFilter::getGameId() const
{
   return mGameId;
}


void PlaybackPacketFilter::setRoundNumber(int number)
{
   mRoundNumber = number;
}


int PlaybackPacketFilter::getRoundNumber() const
{
   return mRoundNumber;
}


bool PlaybackPacketFilter::isEnabled() const
{
   return mEnabled;
}


void PlaybackPacketFilter::setEnabled(bool enabled)
{
   mEnabled = enabled;
}


void PlaybackPacketFilter::setRunning(bool running)
{
   mRunning = running;
}


bool PlaybackPacketFilter::isRunning() const
{
   return mRunning;
}


bool PlaybackPacketFilter::isValid() const
{
   return mGameId != -1;
}


void PlaybackPacketFilter::increaseRoundCounter()
{
   mRoundCounter++;
}


int PlaybackPacketFilter::getRoundCounter() const
{
   return mRoundCounter;
}


bool PlaybackPacketFilter::isRoundReached() const
{
   return getRoundCounter() == getRoundNumber();
}


void PlaybackPacketFilter::reset()
{
   mGameId = 0;
   mRoundNumber = 0;
   mEnabled = false;
   mRunning = false;
   mRoundCounter = 0;
}


