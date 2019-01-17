#ifndef PLAYERSTATS_H
#define PLAYERSTATS_H


class PlayerStats
{

public:

   explicit PlayerStats();

   unsigned int getWins() const;

   unsigned int getKills() const;

   unsigned int getDeaths() const;

   unsigned int getSurvivalTime() const;

   unsigned int getExtrasCollected() const;

   void setWins(unsigned int);

   void setKills(unsigned int);

   void setDeaths(unsigned int);

   void setSurvivalTime(unsigned int time);

   void setExtrasCollected(unsigned int extrasCollected);

   void increaseSurvivalTime(unsigned int);

   void increaseWins();

   void increaseKills();

   void increaseDeaths();

   void increaseExtrasCollected();

   void reset();


protected:

   unsigned int mWins;

   unsigned int mKills;

   unsigned int mDeaths;

   unsigned int mSurvivalTime;

   unsigned int mExtrasCollected;
};

#endif // PLAYERSTATS_H
