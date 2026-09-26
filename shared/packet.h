#ifndef PACKET_H
#define PACKET_H

#include <cstdint>
#include <string>
#include <vector>

#include <QTime>

#include "binaryreader.h"
#include "binarywriter.h"

class Packet : public std::vector<uint8_t>
{
public:
   //! packet types available
   enum TYPE
   {
      INVALID,
      BOMB,
      COUNTDOWN,
      CREATEGAMEREQUEST,
      CREATEGAMERESPONSE,
      DETONATION,
      ERROR,
      EXTRAMAPITEMCREATED,
      GAMEEVENT,
      GAMESTATS,
      JOINGAMEREQUEST,
      JOINGAMERESPONSE,
      KEY,
      LEAVEGAMEREQUEST,
      LEAVEGAMERESPONSE,
      LISTGAMESREQUEST,
      LISTGAMESRESPONSE,
      LOGINREQUEST,
      LOGINRESPONSE,
      MAPCREATEREQUEST,
      MAPITEMCREATED,
      MAPITEMDESTROYED,
      MAPITEMREMOVED,
      MESSAGE,
      MAPITEMMOVE,
      PLAYERKILLED,
      PLAYERMODIFIED,
      POSITION,
      STARTGAMEREQUEST,
      STARTGAMERESPONSE,
      STOPGAMEREQUEST,
      STOPGAMERESPONSE,
      TIME,
      STONEDROP,
      EXTRASHAKE,
      PLAYERSYNCHRONIZEPACKET,
      PLAYERINFECTEDPACKET
   };

   //! read constructor
   Packet();

   //! write constructor
   Packet(TYPE packetType);

   //! destructor
   virtual ~Packet();

   //! serializes a packet
   void serialize();

   //! deserializes a packet
   static Packet* deserialize(BinaryReader&);

   //! debug function
   virtual void debug() = 0;

   //! enqueue member variables
   virtual void enqueue(BinaryWriter&) = 0;

   //! dequeue member variables
   virtual void dequeue(BinaryReader&) = 0;

   //! getter for packet size
   int16_t getSize();

   //! getter for packet type
   TYPE getType();

   //! getter for the packet's timestamp
   const QTime getTimestamp();

   //! timestamp can be modified (e.g. in playback)
   void setTimeStamp(const QTime& time);

   //! getter for packet name
   const std::string& getPacketName() const;

   //! raw byte pointer, kept for existing socket-write call sites
   const char* constData() const;

protected:
   //! packet size
   int16_t mPacketSize;

   //! packet type
   TYPE mPacketType;

   //! timestamp
   QTime mTimestamp;

   //! packet name
   std::string mPacketName;
};

#endif
