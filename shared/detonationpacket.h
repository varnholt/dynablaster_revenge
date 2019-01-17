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
      qint32 x,
      qint32 y,
      qint8 fieldsUp,
      qint8 fieldsDown,
      qint8 fieldsLeft,
      qint8 fieldsRight,
      float intensity
   );

    //! debugs the member variables
    void debug();

    //! enqueues the member variables to datastream
    void enqueue(QDataStream&);

    //! dequeues the member variables from datastream
    void dequeue(QDataStream&);

	//! x center
    qint32 getX() const;

    //! y center
    qint32 getY() const;

    //! number of fields the bomb goes up
    qint8 getUp() const;

    //! number of fields the bomb goes down
    qint8 getDown() const;

    //! number of fields the bomb goes to the left
    qint8 getLeft() const;

    //! number of fields the bomb goes to the right
    qint8 getRight() const;

    //! number of flames / flame intensity
    float getIntensity() const;


private:

    //! x center
    qint32 mX;

    //! y center
    qint32 mY;

    //! number of fields the bomb goes up
    qint8 mFieldsUp;

    //! number of fields the bomb goes down
    qint8 mFieldsDown;

    //! number of fields the bomb goes to the left
    qint8 mFieldsLeft;

    //! number of fields the bomb goes to the right
    qint8 mFieldsRight;

    //! number of flames / intensity
    float mIntensity;
};

#endif // DETONATIONPACKET_H
