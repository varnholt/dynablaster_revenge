#ifndef PLAYERSYNCHRONIZEPACKET_H
#define PLAYERSYNCHRONIZEPACKET_H

#include "packet.h"

class PlayerSynchronizePacket : public Packet
{
   public:

      //! packet can be used for multiple purposes
      enum SynchronizeProcess {
         LevelLoaded,
         Invalid
      };

      //! read constructor
      PlayerSynchronizePacket();

      //! write constructor
      PlayerSynchronizePacket(SynchronizeProcess process);

      //! destructor
      virtual ~PlayerSynchronizePacket();

      //! setter for synchronize process
      void setSynchronizeProcess(SynchronizeProcess process);

      //! getter for synchronize process
      SynchronizeProcess getSynchronizeProcess() const;

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);


   protected:

      //! synchronize process
      SynchronizeProcess mSynchronizeProcess;

};

#endif // PLAYERSYNCHRONIZEPACKET_H
