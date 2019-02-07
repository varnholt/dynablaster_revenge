#pragma once

#include "decode.h"

class Layer1
{
public:
   int process(MpegStream *fr);

   float (*fraction)[SBLIMIT];
};
