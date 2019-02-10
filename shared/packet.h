#ifndef PACKET_H
#define PACKET_H

#include <QByteArray>
#include <QTime>


class Packet : public QByteArray
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
   static Packet* deserialize(QDataStream&);

   //! debug function
   virtual void debug() = 0;

   //! enqueue member variables
   virtual void enqueue(QDataStream&) = 0;

   //! dequeue member variables
   virtual void dequeue(QDataStream&) = 0;

   //! getter for packet size
   qint16 getSize();

   //! getter for packet type
   TYPE getType();

   //! getter for the packet's timestamp
   const QTime getTimestamp();

   //! timestamp can be modified (e.g. in playback)
   void setTimeStamp(const QTime& time);

   //! getter for packet name
   const QString& getPacketName() const;


protected:

   //! packet size
   qint16 mPacketSize;

   //! packet type
   TYPE mPacketType;

   //! timestamp
   QTime mTimestamp;

   //! packet name
   QString mPacketName;
};

#endif
