#pragma once

#include <cstdint>
#include <string>

class Color
{
public:
   Color() = default;
   explicit Color(const std::string& hex_name);
   explicit Color(uint32_t argb);
   Color(int32_t r, int32_t g, int32_t b, int32_t a = 255);

   bool isValid() const;

   int32_t red() const;
   int32_t green() const;
   int32_t blue() const;
   int32_t alpha() const;

   float redF() const;
   float greenF() const;
   float blueF() const;
   float alphaF() const;

   uint32_t rgb() const;
   std::string name() const;

private:
   bool _valid = false;
   int32_t _r = 0;
   int32_t _g = 0;
   int32_t _b = 0;
   int32_t _a = 255;
};
