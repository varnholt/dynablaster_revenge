#pragma once

class MpegStream;

class Id3Tag
{
public:
   Id3Tag();
   ~Id3Tag();

   void reset();
   char* artist() const;
   char* title() const;
   char* album() const;

   int  parse(MpegStream *fr);

private:
   unsigned int headerId(char id[5]) const;

   char* mArtist;
   char* mTitle;
   char* mAlbum;
};

