#pragma once

#include "light.h"

class Stream;

class Spot : public Light
{
public:
   Spot(Node *parent = 0);
   void    load(Stream *stream);
   void    write(Stream *stream);

private:
   int      mShape;
   float    mHotspot;
   float    mFallSize;
};

