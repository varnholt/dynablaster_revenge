#ifndef GAMEEVENTPACKET_H
#define GAMEEVENTPACKET_H

// shared
#include "constants.h"

// shared
#include "packet.h"

class GameEventPacket : public Packet
{
   public:

      //! events
      enum GameEvent
      {
         Invalid,
         BombExploded,
         ExtraCollected,
         ExtraDestroyed
      };

      //! write constructor
      GameEventPacket(
         GameEvent event,
         float intensity = 1.0f,
         int x = -1,
         int y = -1
      );

      //! read constructor
      GameEventPacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

      // BombExplodedPacket

      //! getter for the game event
      GameEvent getGameEvent() const;

      //! getter for the event's intensity
      float getIntensity() const;


      // ExtraCollectedPacket

      //! setter for player id
      void setPlayerId(qint32);

      //! setter for extra type
      void setExtraType(Constants::ExtraType);

      //! getter for player id
      qint32 getPlayerId() const;

      //! getter for extra type
      Constants::ExtraType getExtraType() const;

      //! getter for x position
      int getX() const;

      //! getter for y position
      int getY() const;


   private:

      //! game event
      GameEvent mEvent;

      // TODO: create 2 separate packets here
      //
      // - BombExplodedPacket
      //   +- intensity
      //
      // - ExtraCollectedPacket
      //   +- player id
      //   +- extra type


      //! effect intensity
      float mIntensity;

      // ------- snip --------

      // extra attributes for extra collected

      //! player's id
      qint32 mPlayerId;

      //! extra that was collected
      Constants::ExtraType mExtraType;

      //! affected field x pos
      int mX;

      //! affected field y pos
      int mY;

};

#endif // GAMEEVENTPACKET_H
