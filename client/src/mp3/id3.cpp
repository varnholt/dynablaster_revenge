#include "id3.h"
#include "debug.h"
#include "mpegstream.h"
#include <string.h>

#ifndef __APPLE__
#include <malloc.h>
#else
#include <stdlib.h>
#endif

Id3Tag::Id3Tag()
   : mArtist(0)
   , mTitle(0)
   , mAlbum(0)
{
}

Id3Tag::~Id3Tag()
{
   reset();
}

void Id3Tag::reset()
{
   if (mArtist)
   {
      delete[] mArtist;
      mArtist= 0;
   }

   if (mTitle)
   {
      delete[] mTitle;
      mTitle= 0;
   }

   if (mAlbum)
   {
      delete[] mAlbum;
      mAlbum= 0;
   }
}

char* Id3Tag::artist() const
{
   return mArtist;
}

char* Id3Tag::title() const
{
   return mTitle;
}

char* Id3Tag::album() const
{
   return mAlbum;
}

unsigned int Id3Tag::headerId(char id[5]) const
{
   unsigned int tag= 0;
   for (int i=0;i<4;i++)
   {
      tag= (tag<<8) | id[i];
   }
   return tag;
}

int Id3Tag::parse(MpegStream *fr)
{
   unsigned char version;
   unsigned char flags;
   unsigned int size;

   // the mp3 decoder already read one byte of the version info because it's stupid
   fr->fullread(&version, 1); // rest of version
   fr->fullread(&flags, 1); // flags
   fr->fullread((unsigned char*)&size, 4);
   size= (size>>24&0xff)|(size>>8&0xff00)|(size<<8&0xff0000)|(size<<24&0xff000000);

   if (flags & 0x40) // extended header?
   {
      unsigned int size;
      fr->fullread((unsigned char*)&size, 4);
   }

   unsigned int header;
   do
   {
      int size;
      unsigned char text[256];
      fr->fullread((unsigned char*)&header, 4);
      if (header)
      {
         int enc=0;
         fr->fullread((unsigned char*)&size, 4);
         fr->fullread((unsigned char*)&enc, 3);
         header= (header>>24&0xff)|(header>>8&0xff00)|(header<<8&0xff0000)|(header<<24&0xff000000);
         size= (size>>24&0xff)|(size>>8&0xff00)|(size<<8&0xff0000)|(size<<24&0xff000000);
         if (size>0 && size<256)
         {
            fr->fullread(text, size-1);
            text[size-1]= 0;
            if (enc >> 16 & 1)
            {
               // utf-irgendwas. jedes zweite byte schmeissen wir mal weg.
               size= (size/2);
               for (int i=0;i<size;i++)
                  text[i]= text[i*2+2];
            }
            //printf("%c%c%c%c: %s\n", header>>24&255, header>>16&255, header>>8&255, header&255, text);

            if (header == headerId("TPE1"))
            {
               if (mArtist)
                  delete[] mArtist;
               mArtist= new char[size];
               memcpy(mArtist, text, size);
            }

            if (header == headerId("TPE2"))
            {
               if (mArtist)
                  delete[] mArtist;
               mArtist= new char[size];
               memcpy(mArtist, text, size);
            }

            if (header == headerId("TIT2"))
            {
               if (mTitle)
                  delete[] mTitle;
               mTitle= new char[size];
               memcpy(mTitle, text, size);
            }

            if (header == headerId("TALB"))
            {
               if (mAlbum)
                  delete[] mAlbum;
               mAlbum= new char[size];
               memcpy(mAlbum, text, size);
            }
         }
         else
            header= 0;
      }
   } while (header);

   return 0;
}

