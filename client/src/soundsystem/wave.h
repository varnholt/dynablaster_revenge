#ifndef WAVE_H
#define WAVE_H

class Stream;

class Wave
{
public:

   struct Header
   {
      unsigned int   riff;
      unsigned int   filesize;
      unsigned int   wave;
      unsigned int   fmt;
      unsigned int   fmtsize;
      unsigned short format;
      unsigned short channels;
      unsigned int   rate;
      unsigned int   bps;
      unsigned short align;
      unsigned short bits;
      unsigned int   data;
      int            datasize;

      bool load(Stream *stream);
   };


};

#endif
