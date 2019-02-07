#pragma once

#include "light.h"

class Stream;

class Omni : public Light
{
public:
           Omni(Node *parent = 0);
   void    load(Stream *stream);
   void    write(Stream *stream);

private:
   float    mAttNear;
   float    mAttFar;
};

