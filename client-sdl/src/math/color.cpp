#include "color.h"

#include <cstdio>

namespace
{
int32_t hexPair(const std::string& hex_name, size_t offset)
{
   return std::stoi(hex_name.substr(offset, 2), nullptr, 16);
}
}  // namespace

Color::Color(const std::string& hex_name)
{
   if (hex_name.size() != 7 || hex_name[0] != '#')
   {
      return;
   }

   try
   {
      _r = hexPair(hex_name, 1);
      _g = hexPair(hex_name, 3);
      _b = hexPair(hex_name, 5);
      _a = 255;
      _valid = true;
   }
   catch (const std::exception&)
   {
      _valid = false;
   }
}

Color::Color(uint32_t argb)
    : _valid(true),
      _r(static_cast<int32_t>((argb >> 16) & 0xff)),
      _g(static_cast<int32_t>((argb >> 8) & 0xff)),
      _b(static_cast<int32_t>(argb & 0xff)),
      _a(static_cast<int32_t>((argb >> 24) & 0xff))
{
}

Color::Color(int32_t r, int32_t g, int32_t b, int32_t a) : _valid(true), _r(r), _g(g), _b(b), _a(a)
{
}

bool Color::isValid() const
{
   return _valid;
}

int32_t Color::red() const
{
   return _r;
}

int32_t Color::green() const
{
   return _g;
}

int32_t Color::blue() const
{
   return _b;
}

int32_t Color::alpha() const
{
   return _a;
}

float Color::redF() const
{
   return static_cast<float>(_r) / 255.0f;
}

float Color::greenF() const
{
   return static_cast<float>(_g) / 255.0f;
}

float Color::blueF() const
{
   return static_cast<float>(_b) / 255.0f;
}

float Color::alphaF() const
{
   return static_cast<float>(_a) / 255.0f;
}

uint32_t Color::rgb() const
{
   return (static_cast<uint32_t>(_a) << 24) | (static_cast<uint32_t>(_r) << 16) | (static_cast<uint32_t>(_g) << 8) |
          static_cast<uint32_t>(_b);
}

std::string Color::name() const
{
   char buffer[8];
   std::snprintf(buffer, sizeof(buffer), "#%02x%02x%02x", _r, _g, _b);
   return buffer;
}
