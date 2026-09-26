#ifndef GAMESTATSPACKET_H
#define GAMESTATSPACKET_H

#include "packet.h"

#include "playerstats.h"

class GameStatsPacket : public Packet
{
public:
   //! write constructor
   GameStatsPacket(const QList<int>& ids, const QList<PlayerStats>& overallStats, const QList<PlayerStats>& roundStats);

   //! read constructor
   GameStatsPacket();

   //! destructor
   virtual ~GameStatsPacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(BinaryWriter&);

   //! dequeues the member variables from datastream
   void dequeue(BinaryReader&);

   //! getter for the overall player stats
   QList<PlayerStats> getOverallStats() const;

   //! getter for the round player stats
   QList<PlayerStats> getRoundStats() const;

   //! getter for list of player ids
   QList<int> getPlayerIds() const;

private:
   //! dequeue stats list
   void dequeueStatsList(BinaryReader& in, int size, QList<PlayerStats>* list);

   //! list of player ids
   QList<int> mPlayerIds;

   //! player stats to send
   QList<PlayerStats> mOverallStats;

   //! player rounds stats to send
   QList<PlayerStats> mRoundStats;
};

#endif  // GAMESTATSPACKET_H
