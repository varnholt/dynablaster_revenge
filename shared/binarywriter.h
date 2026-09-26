#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <QList>
#include <QString>
#include <QTime>

#include "point.h"

// appends fixed-width, little-endian values to a growable byte buffer, mirroring
// QDataStream's operator<< shape so packet enqueue() bodies barely change. Also knows how to
// write the handful of Qt types (QString/QList<T>/QTime) packet fields still carry -
// those types themselves aren't going away in this pass, only QDataStream/QByteArray are.
class BinaryWriter
{
public:
   explicit BinaryWriter(std::vector<uint8_t>& buffer);

   BinaryWriter& operator<<(int8_t value);
   BinaryWriter& operator<<(uint8_t value);
   BinaryWriter& operator<<(int16_t value);
   BinaryWriter& operator<<(uint16_t value);
   BinaryWriter& operator<<(int32_t value);
   BinaryWriter& operator<<(uint32_t value);
   BinaryWriter& operator<<(int64_t value);
   BinaryWriter& operator<<(uint64_t value);
   BinaryWriter& operator<<(float value);
   BinaryWriter& operator<<(double value);
   BinaryWriter& operator<<(bool value);
   BinaryWriter& operator<<(const std::string& value);
   BinaryWriter& operator<<(const QString& value);
   BinaryWriter& operator<<(const Point& value);
   BinaryWriter& operator<<(const QTime& value);

   template <typename T>
   BinaryWriter& operator<<(const QList<T>& list)
   {
      *this << static_cast<uint32_t>(list.size());

      for (const auto& item : list)
      {
         *this << item;
      }

      return *this;
   }

   template <typename T>
   BinaryWriter& operator<<(const std::vector<T>& list)
   {
      *this << static_cast<uint32_t>(list.size());

      for (const auto& item : list)
      {
         *this << item;
      }

      return *this;
   }

   size_t pos() const;

   // patches a uint16_t already written at 'offset', used for the leading packet-size
   // field that's only known once the whole payload has been enqueued
   void patchUint16(size_t offset, uint16_t value);

private:
   template <typename T>
   void write(T value);

   std::vector<uint8_t>& _buffer;
};
