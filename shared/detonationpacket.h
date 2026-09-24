#ifndef DETONATIONPACKET_H
#define DETONATIONPACKET_H

#include "packet.h"

class DetonationPacket : public Packet
{

public:

   //! read constructor
   DetonationPacket();

   //! write constructor
   DetonationPacket(
      int32_t x,
      int32_t y,
      int8_t fieldsUp,
      int8_t fieldsDown,
      int8_t fieldsLeft,
      int8_t fieldsRight,
      float intensity
   );

    //! debugs the member variables
    void debug();

    //! enqueues the member variables to datastream
    void enqueue(QDataStream&);

    //! dequeues the member variables from datastream
    void dequeue(QDataStream&);

	//! x center
    int32_t getX() const;

    //! y center
    int32_t getY() const;

    //! number of fields the bomb goes up
    int8_t getUp() const;

    //! number of fields the bomb goes down
    int8_t getDown() const;

    //! number of fields the bomb goes to the left
    int8_t getLeft() const;

    //! number of fields the bomb goes to the right
    int8_t getRight() const;

    //! number of flames / flame intensity
    float getIntensity() const;


private:

    //! x center
    int32_t mX;

    //! y center
    int32_t mY;

    //! number of fields the bomb goes up
    int8_t mFieldsUp;

    //! number of fields the bomb goes down
    int8_t mFieldsDown;

    //! number of fields the bomb goes to the left
    int8_t mFieldsLeft;

    //! number of fields the bomb goes to the right
    int8_t mFieldsRight;

    //! number of flames / intensity
    float mIntensity;
};

#endif // DETONATIONPACKET_H
