#include "playerstats.h"

PlayerStats::PlayerStats()
 : mWins(0),
   mKills(0),
   mDeaths(0),
   mSurvivalTime(0),
   mExtrasCollected(0)
{
}


void PlayerStats::increaseWins()
{
   mWins++;
}


void PlayerStats::increaseKills()
{
   mKills++;
}


void PlayerStats::increaseDeaths()
{
   mDeaths++;
}


void PlayerStats::increaseExtrasCollected()
{
   mExtrasCollected++;
}


void PlayerStats::reset()
{
   mWins = 0;
   mKills = 0;
   mDeaths = 0;
   mSurvivalTime = 0;
   mExtrasCollected = 0;
}


void PlayerStats::setWins(unsigned int wins)
{
   mWins = wins;
}


void PlayerStats::setKills(unsigned int kills)
{
   mKills = kills;
}


void PlayerStats::setDeaths(unsigned int deaths)
{
   mDeaths = deaths;
}


void PlayerStats::setSurvivalTime(unsigned int time)
{
   mSurvivalTime = time;
}


void PlayerStats::setExtrasCollected(unsigned int extrasCollected)
{
   mExtrasCollected = extrasCollected;
}


unsigned int PlayerStats::getWins() const
{
   return mWins;
}


unsigned int PlayerStats::getKills() const
{
   return mKills;
}


unsigned int PlayerStats::getDeaths() const
{
   return mDeaths;
}


void PlayerStats::increaseSurvivalTime(unsigned int survivalTime)
{
   mSurvivalTime += survivalTime;
}


unsigned int PlayerStats::getSurvivalTime() const
{
   return mSurvivalTime;
}


unsigned int PlayerStats::getExtrasCollected() const
{
   return mExtrasCollected;
}

