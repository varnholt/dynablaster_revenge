#include "packetstreambuffer.h"


//-----------------------------------------------------------------------------
PacketStreamBuffer::PacketStreamBuffer()
{
   mDevice.setBuffer(&mBuffer);
   mDevice.open(QIODevice::ReadWrite);

   mStream.setDevice(&mDevice);
   mStream.setVersion(QDataStream::Qt_4_6);
}


//-----------------------------------------------------------------------------
/*!
   \param data raw bytes just read from the transport
   \param length number of bytes in \c data
*/
void PacketStreamBuffer::append(const char* data, qint64 length)
{
   mBuffer.append(data, static_cast<qsizetype>(length));
}


//-----------------------------------------------------------------------------
qint64 PacketStreamBuffer::bytesAvailable() const
{
   return mBuffer.size() - mDevice.pos();
}


//-----------------------------------------------------------------------------
QDataStream& PacketStreamBuffer::stream()
{
   return mStream;
}


//-----------------------------------------------------------------------------
void PacketStreamBuffer::compact()
{
   const auto pos = mDevice.pos();

   if (pos > 0)
   {
      mBuffer.remove(0, static_cast<qsizetype>(pos));
      mDevice.seek(0);
   }
}
