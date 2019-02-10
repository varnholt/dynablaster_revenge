// header
#include "gamestatspacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "GameStats"

//----------------------------------------------------------------------------
/*!
   \param message message to send
   \param receiverId id of the receiver
*/
GameStatsPacket::GameStatsPacket(
   const QList<int>& ids,
   const QList<PlayerStats>& overallStats,
   const QList<PlayerStats>& roundStats
)
 : Packet(Packet::GAMESTATS),
   mPlayerIds(ids),
   mOverallStats(overallStats),
   mRoundStats(roundStats)
{
   mPacketName = PACKETNAME;
}


//----------------------------------------------------------------------------
/*!
*/
GameStatsPacket::GameStatsPacket()
   : Packet(Packet::GAMESTATS)
{
   mPacketName = PACKETNAME;
}


//----------------------------------------------------------------------------
/*!
*/
GameStatsPacket::~GameStatsPacket()
{
}


//----------------------------------------------------------------------------
/*!
   \return overall game stats
*/
QList<PlayerStats> GameStatsPacket::getOverallStats() const
{
   return mOverallStats;
}


//----------------------------------------------------------------------------
/*!
   \return round game stats
*/
QList<PlayerStats> GameStatsPacket::getRoundStats() const
{
   return mRoundStats;
}


//----------------------------------------------------------------------------
/*!
   \return player ids
*/
QList<int> GameStatsPacket::getPlayerIds() const
{
   return mPlayerIds;
}


//----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void GameStatsPacket::enqueue(QDataStream & out)
{
   // write size
   out << mOverallStats.size();

   // write list of ids
   foreach(int id, mPlayerIds)
   {
      out << id;
   }

   QList< QList<PlayerStats>* > statLists;
   statLists << &mOverallStats;
   statLists << &mRoundStats;

   // write overall stats
   foreach(QList<PlayerStats>* list, statLists)
   {
      foreach(PlayerStats stats, *list)
      {
         out
            << stats.getWins()
            << stats.getKills()
            << stats.getDeaths()
            << stats.getSurvivalTime()
            << stats.getExtrasCollected();
      }
   }
}


//----------------------------------------------------------------------------
/*!
   \param in datastream read members from
   \param size list size
   \param list list to fill with values
*/
void GameStatsPacket::dequeueStatsList(
   QDataStream& in,
   int size,
   QList<PlayerStats>* list
)
{
   unsigned int wins = 0;
   unsigned int kills = 0;
   unsigned int deaths = 0;
   unsigned int survivalTime = 0;
   unsigned int extrasCollected = 0;

   for (int i = 0; i < size; i++)
   {
      PlayerStats stats;

      in
         >> wins
         >> kills
         >> deaths
         >> survivalTime
         >> extrasCollected;

      stats.setWins(wins);
      stats.setKills(kills);
      stats.setDeaths(deaths);
      stats.setSurvivalTime(survivalTime);
      stats.setExtrasCollected(extrasCollected);

      list->append(stats);
   }
}


//----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void GameStatsPacket::dequeue(QDataStream & in)
{
   // read size
   int size = 0;
   in >> size;

   // read list of ids
   int id = 0;
   for (int i = 0; i < size; i++)
   {
      in >> id;
      mPlayerIds.push_back(id);
   }

   // read list of stats
   dequeueStatsList(in, size, &mOverallStats);
   dequeueStatsList(in, size, &mRoundStats);
}


//----------------------------------------------------------------------------
/*!
   debug output of members
*/
void GameStatsPacket::debug()
{
   // debug message request
   qDebug(
      "GameStatsPacket:debug: "
   );
}


