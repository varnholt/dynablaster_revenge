#ifndef PACKETSTREAMBUFFER_H
#define PACKETSTREAMBUFFER_H

#include <cstddef>
#include <cstdint>
#include <vector>

#include "binaryreader.h"

/*!
   Buffers raw bytes read from a non-blocking transport and hands out a BinaryReader positioned
   at the first unconsumed byte, so the length-prefix reassembly loop in Server::readSocket() /
   BombermanClient's socket handling can attempt a read, then commit only the bytes it actually
   consumed via consume().
*/
class PacketStreamBuffer
{
public:
   //! append raw bytes just read from the transport
   void append(const char* data, size_t length);

   //! bytes not yet consumed from the stream
   size_t bytesAvailable() const;

   //! reader positioned at the first unconsumed byte
   BinaryReader reader() const;

   //! commit that 'bytes' bytes were consumed from the current read position
   void consume(size_t bytes);

   //! drop already-consumed bytes so the buffer doesn't grow without bound; call once a
   //! caller's reassembly loop has drained everything it currently can
   void compact();

private:
   std::vector<uint8_t> mBuffer;
   size_t mPos = 0;
};

#endif
