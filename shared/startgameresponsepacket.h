#ifndef STARTGAMERESPONSEPACKET_H
#define STARTGAMERESPONSEPACKET_H

#include "packet.h"

// Qt
#include <QString>

class StartGameResponsePacket : public Packet
{

   public:

      //! write constructor, -1 if not accepted
      StartGameResponsePacket(int id, bool started);

      //! read constructor
      StartGameResponsePacket();

      //! destructor
      virtual ~StartGameResponsePacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

      //! getter for game id
      int getId();

      //! getter for game is started flag
      bool isStarted();


   private:

      //! game id
      qint32 mId;

      //! game was started
      bool mStarted;
};

#endif // STARTGAMERESPONSEPACKET_H
