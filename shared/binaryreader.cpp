#include "binaryreader.h"

#include <cstring>

BinaryReader::BinaryReader(const uint8_t* data, size_t size) : _data(data), _size(size)
{
}

BinaryReader::BinaryReader(const std::vector<uint8_t>& buffer) : _data(buffer.data()), _size(buffer.size())
{
}

template <typename T>
T BinaryReader::read()
{
   T value{};

   if (!_ok)
   {
      return value;
   }

   if (_pos + sizeof(T) > _size)
   {
      _ok = false;
      return value;
   }

   std::memcpy(&value, _data + _pos, sizeof(T));
   _pos += sizeof(T);
   return value;
}

BinaryReader& BinaryReader::operator>>(int8_t& value)
{
   value = read<int8_t>();
   return *this;
}

BinaryReader& BinaryReader::operator>>(uint8_t& value)
{
   value = read<uint8_t>();
   return *this;
}

BinaryReader& BinaryReader::operator>>(int16_t& value)
{
   value = read<int16_t>();
   return *this;
}

BinaryReader& BinaryReader::operator>>(uint16_t& value)
{
   value = read<uint16_t>();
   return *this;
}

BinaryReader& BinaryReader::operator>>(int32_t& value)
{
   value = read<int32_t>();
   return *this;
}

BinaryReader& BinaryReader::operator>>(uint32_t& value)
{
   value = read<uint32_t>();
   return *this;
}

BinaryReader& BinaryReader::operator>>(int64_t& value)
{
   value = read<int64_t>();
   return *this;
}

BinaryReader& BinaryReader::operator>>(uint64_t& value)
{
   value = read<uint64_t>();
   return *this;
}

BinaryReader& BinaryReader::operator>>(float& value)
{
   value = read<float>();
   return *this;
}

BinaryReader& BinaryReader::operator>>(double& value)
{
   value = read<double>();
   return *this;
}

BinaryReader& BinaryReader::operator>>(bool& value)
{
   value = read<uint8_t>() != 0;
   return *this;
}

BinaryReader& BinaryReader::operator>>(std::string& value)
{
   const auto length = read<uint32_t>();

   if (!_ok || _pos + length > _size)
   {
      _ok = false;
      value.clear();
      return *this;
   }

   value.assign(reinterpret_cast<const char*>(_data + _pos), length);
   _pos += length;
   return *this;
}

BinaryReader& BinaryReader::operator>>(QString& value)
{
   const auto length = read<uint32_t>();

   if (!_ok || _pos + length > _size)
   {
      _ok = false;
      value.clear();
      return *this;
   }

   value = QString::fromUtf8(reinterpret_cast<const char*>(_data + _pos), static_cast<qsizetype>(length));
   _pos += length;
   return *this;
}

BinaryReader& BinaryReader::operator>>(Point& value)
{
   int32_t x = 0;
   int32_t y = 0;
   *this >> x >> y;
   value.setX(x);
   value.setY(y);
   return *this;
}

BinaryReader& BinaryReader::operator>>(QTime& value)
{
   int32_t msecs = 0;
   *this >> msecs;
   value = QTime::fromMSecsSinceStartOfDay(msecs);
   return *this;
}

bool BinaryReader::ok() const
{
   return _ok;
}

size_t BinaryReader::pos() const
{
   return _pos;
}

size_t BinaryReader::bytesAvailable() const
{
   return _size - _pos;
}
