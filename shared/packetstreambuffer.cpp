#include "packetstreambuffer.h"

#include <cstring>

//-----------------------------------------------------------------------------
/*!
   \param data raw bytes just read from the transport
   \param length number of bytes in \c data
*/
void PacketStreamBuffer::append(const char* data, size_t length)
{
   const auto offset = mBuffer.size();
   mBuffer.resize(offset + length);
   std::memcpy(mBuffer.data() + offset, data, length);
}

//-----------------------------------------------------------------------------
size_t PacketStreamBuffer::bytesAvailable() const
{
   return mBuffer.size() - mPos;
}

//-----------------------------------------------------------------------------
BinaryReader PacketStreamBuffer::reader() const
{
   return BinaryReader(mBuffer.data() + mPos, mBuffer.size() - mPos);
}

//-----------------------------------------------------------------------------
/*!
   \param bytes number of bytes a caller's reader() actually consumed
*/
void PacketStreamBuffer::consume(size_t bytes)
{
   mPos += bytes;
}

//-----------------------------------------------------------------------------
void PacketStreamBuffer::compact()
{
   if (mPos > 0)
   {
      mBuffer.erase(mBuffer.begin(), mBuffer.begin() + static_cast<std::ptrdiff_t>(mPos));
      mPos = 0;
   }
}
