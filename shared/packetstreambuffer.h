#ifndef PACKETSTREAMBUFFER_H
#define PACKETSTREAMBUFFER_H

#include <QBuffer>
#include <QByteArray>
#include <QDataStream>


/*!
   Buffers raw bytes read from a non-blocking transport and exposes them as a QDataStream,
   so the length-prefix reassembly loop in Server::data() / BombermanClient's socket handling
   can read frames the same way regardless of what's feeding the bytes in.
*/
class PacketStreamBuffer
{

public:

   PacketStreamBuffer();

   //! append raw bytes just read from the transport
   void append(const char* data, qint64 length);

   //! bytes not yet consumed from the stream
   qint64 bytesAvailable() const;

   //! data stream positioned at the first unconsumed byte
   QDataStream& stream();

   //! drop already-consumed bytes so the buffer doesn't grow without bound; call once a
   //! caller's reassembly loop has drained everything it currently can
   void compact();

private:

   QByteArray mBuffer;
   QBuffer mDevice;
   QDataStream mStream;
};

#endif
