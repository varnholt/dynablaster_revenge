#pragma once

#include "decode.h"

class Layer2
{
public:
   void init();
   int process(MpegStream *fr);

   int II_sblimit;
   float (*fraction)[4][SBLIMIT]; /* ALIGNED(16) real fraction[2][4][SBLIMIT] */
};
