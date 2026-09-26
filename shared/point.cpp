#include "point.h"

#include <cstdlib>

Point::Point(int x, int y) : _x(x), _y(y)
{
}

int Point::x() const
{
   return _x;
}

int Point::y() const
{
   return _y;
}

void Point::setX(int x)
{
   _x = x;
}

void Point::setY(int y)
{
   _y = y;
}

bool Point::operator==(const Point& other) const
{
   return _x == other._x && _y == other._y;
}

bool Point::operator!=(const Point& other) const
{
   return !(*this == other);
}

Point Point::operator-(const Point& other) const
{
   return Point(_x - other._x, _y - other._y);
}

Point Point::operator-() const
{
   return Point(-_x, -_y);
}

bool Point::isNull() const
{
   return _x == 0 && _y == 0;
}

int Point::manhattanLength() const
{
   return std::abs(_x) + std::abs(_y);
}
