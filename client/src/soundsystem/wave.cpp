#include "wave.h"
#include "tools/stream.h"

bool Wave::Header::load(Stream *stream)
{
   riff= stream->getInt();
   if (riff!=0x46464952) // "RIFF"
      return false;

   filesize= stream->getInt();
   // TODO: sanity check

   wave= stream->getInt();
   if (wave!=0x45564157) // "WAVE"
      return false;

   fmt= stream->getInt();
   if (fmt != 0x20746d66) // "fmt "
      return false;

   fmtsize= stream->getInt();
   if (fmtsize != 16)
      return false;

   format= stream->getWord();
   if (format != 1)
      return false;

   channels= stream->getWord();

   rate= stream->getInt();
   
   bps= stream->getInt();

   align= stream->getWord();

   bits= stream->getWord();

   data= stream->getInt();
   if (data != 0x61746164)
      return false;

   datasize= stream->getInt();

   return true;
}

