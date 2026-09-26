#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <QList>
#include <QPoint>
#include <QString>
#include <QTime>

// reads fixed-width, little-endian values back out of a byte buffer, mirroring
// QDataStream's operator>> shape; a read past the end leaves the value untouched
// and latches 'ok' to false instead of asserting, since packet bytes come off the
// network and a truncated/malformed packet must fail gracefully, not crash. Also knows how
// to read the handful of Qt types (QString/QPoint/QList<T>/QTime) packet fields still carry -
// those types themselves aren't going away in this pass, only QDataStream/QByteArray are.
class BinaryReader
{
public:
   BinaryReader(const uint8_t* data, size_t size);
   explicit BinaryReader(const std::vector<uint8_t>& buffer);

   BinaryReader& operator>>(int8_t& value);
   BinaryReader& operator>>(uint8_t& value);
   BinaryReader& operator>>(int16_t& value);
   BinaryReader& operator>>(uint16_t& value);
   BinaryReader& operator>>(int32_t& value);
   BinaryReader& operator>>(uint32_t& value);
   BinaryReader& operator>>(int64_t& value);
   BinaryReader& operator>>(uint64_t& value);
   BinaryReader& operator>>(float& value);
   BinaryReader& operator>>(double& value);
   BinaryReader& operator>>(bool& value);
   BinaryReader& operator>>(std::string& value);
   BinaryReader& operator>>(QString& value);
   BinaryReader& operator>>(QPoint& value);
   BinaryReader& operator>>(QTime& value);

   template <typename T>
   BinaryReader& operator>>(QList<T>& list)
   {
      uint32_t count = 0;
      *this >> count;

      list.clear();

      for (uint32_t i = 0; i < count && _ok; ++i)
      {
         T item{};
         *this >> item;
         list.push_back(item);
      }

      return *this;
   }

   bool ok() const;
   size_t pos() const;
   size_t bytesAvailable() const;

private:
   template <typename T>
   T read();

   const uint8_t* _data;
   size_t _size;
   size_t _pos = 0;
   bool _ok = true;
};
