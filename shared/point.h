#pragma once

#include <cstddef>
#include <functional>

// integer grid coordinate, mirroring QPoint's accessor shape
class Point
{
public:
   Point() = default;

   Point(int x, int y);

   int x() const;
   int y() const;

   void setX(int x);
   void setY(int y);

   bool operator==(const Point& other) const;
   bool operator!=(const Point& other) const;

   Point operator-(const Point& other) const;
   Point operator-() const;

   bool isNull() const;
   int manhattanLength() const;

private:
   int _x = 0;
   int _y = 0;
};

template <>
struct std::hash<Point>
{
   // boost-style hash_combine of the two components
   std::size_t operator()(const Point& point) const noexcept
   {
      std::size_t h1 = std::hash<int>{}(point.x());
      std::size_t h2 = std::hash<int>{}(point.y());
      return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
   }
};
