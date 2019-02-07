#pragma once

class AudioFormat
{
public:
   int encoding;
   int encsize;
   int channels;
   long rate;

   void invalidate();
};

