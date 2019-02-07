// header
#include "openaloggloader.h"

// Qt
#include <QObject>
#include <QStringList>
#include <QtDebug>

// defines
#define BUFFER_SIZE (4096 * 4)

#ifdef WIN32

// http://lists.xiph.org/pipermail/vorbis/2005-April/025755.html
// http://www.gamedev.net/topic/464364-how-to-get-ogg-vorbis-libraries-working/
// http://www.dsource.org/forums/viewtopic.php?t=1338&sid=0dddaceb80efc679274ab4272c059346
// http://jgge.googlecode.com/svn-history/r10/trunk/Programming/osgal-0.6.1/src/openalpp/FileStream.cpp

size_t VorbisRead(
   void *ptr, 
   size_t byteSize, 
   size_t sizeToRead, 
   void *datasource
)
{
   FILE* file = (FILE*)datasource;
   size_t bytes_read = fread(ptr, byteSize, sizeToRead, file);
   return bytes_read; //actualSizeToRead;
}


int VorbisSeek(
   void *datasource, 
   ogg_int64_t offset, 
   int whence
)
{
   FILE* file = (FILE*)datasource;
   return fseek(file, (long)offset, whence );
}


int VorbisClose(void *datasource)
{
   FILE* file = (FILE*)datasource;
   return fclose(file);
}


long VorbisTell(void *datasource)
{
   FILE* file = (FILE*)datasource;
   return ftell(file);
}

#endif

OpenAlOggLoader::OpenAlOggLoader()
   : mFile(0),
     mVorbisInfo(0),
     mVorbisComment(0),
     mSource(0),
     mFormat(0)
{
   mBuffers[0]=0;
   mBuffers[1]=0;
}


OpenAlOggLoader::~OpenAlOggLoader()
{
   release();
}


void OpenAlOggLoader::open(const QString& path)
{
   int result = 0;

   mFile = fopen(qPrintable(path), "rb");

#ifdef WIN32

   if (mFile)
   {
      ov_callbacks vorbisCallbacks;
      vorbisCallbacks.read_func  = &VorbisRead;
      vorbisCallbacks.close_func = &VorbisClose;
      vorbisCallbacks.seek_func  = &VorbisSeek;
      vorbisCallbacks.tell_func  = &VorbisTell;

      ov_open_callbacks(mFile, &mOggStream, 0, 0, vorbisCallbacks);
   }
   else
   {
      qWarning("OggLoader::open: could not open ogg file.");
   }

#else

   if (mFile)
   {
      if ((result = ov_open(mFile, &mOggStream, NULL, 0)) < 0)
      {
         fclose(mFile);

         qWarning("OggLoader::open: could not open ogg stream. %s",
            qPrintable(errorString(result))
         );
      }
   }
   else
   {
      qWarning("OggLoader::open: could not open ogg file.");
   }

#endif

   mVorbisInfo = ov_info(&mOggStream, -1);
   mVorbisComment = ov_comment(&mOggStream, -1);

   parseCommentData();

   if(mVorbisInfo->channels == 1)
      mFormat = AL_FORMAT_MONO16;
   else
      mFormat = AL_FORMAT_STEREO16;

   alGenBuffers(2, mBuffers);
   check();
   alGenSources(1, &mSource);
   check();

   alSource3f(mSource, AL_POSITION,        0.0, 0.0, 0.0);
   alSource3f(mSource, AL_VELOCITY,        0.0, 0.0, 0.0);
   alSource3f(mSource, AL_DIRECTION,       0.0, 0.0, 0.0);
   alSourcef (mSource, AL_ROLLOFF_FACTOR,  0.0          );
   alSourcei (mSource, AL_SOURCE_RELATIVE, AL_TRUE      );

   // notify listeners
   emit playing(
      mArtist,
      mAlbum,
      mTrack
   );
}


void OpenAlOggLoader::release()
{
    alSourceStop(mSource);
    empty();
    alDeleteSources(1, &mSource);
    check();
    alDeleteBuffers(1, mBuffers);
    check();

    // FIX THIS SHIT!
//    ov_clear(&mOggStream);
}


void OpenAlOggLoader::display()
{
   qDebug()
      << "version:         " << mVorbisInfo->version         << "\n"
      << "channels:        " << mVorbisInfo->channels        << "\n"
      << "rate (hz):       " << mVorbisInfo->rate            << "\n"
      << "bitrate upper:   " << mVorbisInfo->bitrate_upper   << "\n"
      << "bitrate nominal: " << mVorbisInfo->bitrate_nominal << "\n"
      << "bitrate lower:   " << mVorbisInfo->bitrate_lower   << "\n"
      << "bitrate window:  " << mVorbisInfo->bitrate_window  << "\n"
      << "vendor:          " << mVorbisComment->vendor       << "\n";

    for (int i = 0; i < mVorbisComment->comments; i++)
    {
        qDebug()
            << "comment ("
            << i
            << "):     "
            << mVorbisComment->user_comments[i];
    }
        
    qDebug(" ");
}


void OpenAlOggLoader::parseCommentData()
{
   QMap<QString, QString> comments;
   QString key;
   QString value;
   for (int i = 0; i < mVorbisComment->comments; i++)
   {
      QStringList list =
         QString(mVorbisComment->user_comments[i]).split('=');

      key = list.first();
      list.removeFirst();

      // in case it was split
      value = list.join("=");

      comments.insert(key, value);
   }

   mArtist = comments["ARTIST"];
   mTrack = comments["TITLE"];
   mAlbum = comments["ALBUM"];

   qDebug(
      "OpenAlOggLoader::parseCommentData:\n"
      "  artist: %s\n"
      "  title: %s\n"
      "  album: %s",
      qPrintable(mArtist),
      qPrintable(mTrack),
      qPrintable(mAlbum)
   );
}


bool OpenAlOggLoader::play()
{
    if(isPlaying())
        return true;
        
    if(!stream(mBuffers[0]))
        return false;
        
    if(!stream(mBuffers[1]))
        return false;
    
    alSourceQueueBuffers(mSource, 2, mBuffers);
    alSourcePlay(mSource);

    return true;
}


bool OpenAlOggLoader::isPlaying()
{
    ALenum state;
    
    alGetSourcei(mSource, AL_SOURCE_STATE, &state);
    
    return (state == AL_PLAYING);
}


bool OpenAlOggLoader::update()
{
   mMutex.lock();

   int processed = 0;
   bool active = true;

   alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);

   while (processed > 0)
   {
      ALuint buffer;

      alSourceUnqueueBuffers(mSource, 1, &buffer);
      check();

      active = stream(buffer);

      alSourceQueueBuffers(mSource, 1, &buffer);
      check();

      processed--;
   }

   mMutex.unlock();

   return active;
}


bool OpenAlOggLoader::stream(ALuint buffer)
{
    char pcm[BUFFER_SIZE];
    int  size = 0;
    int  section;
    int  result;

   while(size < BUFFER_SIZE)
   {
      result = ov_read(
         &mOggStream,
         pcm + size,
         BUFFER_SIZE - size,
         0,
         2,
         1,
         &section
      );

      if(result > 0)
      {
         size += result;
      }
      else
      {
         if(result < 0)
         {
            qWarning("%s", qPrintable(errorString(result)));
         }
         else
         {
            break;
         }
      }
   }
    
    if(size == 0)
        return false;

    alBufferData(buffer, mFormat, pcm, size, mVorbisInfo->rate);
    check();
    
    return true;
}


void OpenAlOggLoader::empty()
{
    int queued = 0;
    alGetSourcei(mSource, AL_BUFFERS_QUEUED, &queued);
    
    while(queued--)
    {
        ALuint buffer = 0;
        alSourceUnqueueBuffers(mSource, 1, &buffer);

        check();
    }
}


void OpenAlOggLoader::check()
{
   int error = alGetError();

   if (error != AL_NO_ERROR)
   {
      qDebug(
         "OggLoader::check(): OpenAL error was raised: %s (%d)",
         qPrintable(errorString(error)),
         error
      );
   }
}


QString OpenAlOggLoader::errorString(int code)
{
    switch(code)
    {
        case OV_EREAD:
            return "Read from media.";
        case OV_ENOTVORBIS:
            return "Not Vorbis data.";
        case OV_EVERSION:
            return "Vorbis version mismatch.";
        case OV_EBADHEADER:
            return "Invalid Vorbis header.";
        case OV_EFAULT:
            return "Internal logic fault (bug or heap/stack corruption.";
        default:
            return "Unknown Ogg error.";
    }
}




// -------------------------------------------------------------------------- //


//class OggBuffer
//{
//   public:
//
//      //! the ogg data
//      QByteArray* mBuffer;
//
//      //! number of bytes read
//      int mBytesRead;
//};


// ugly memory loader
// openfrommemory
//!
// void openFromMemory(QByteArray* array);


// ugly memory loader
// OggBuffer mOggMemoryFile;


/*
size_t vorbisRead(
   void *ptr,
   size_t byteSize,
   size_t sizeToRead,
   void *datasource
)
{
   // How much more we can read till we hit the EOF marker
   size_t bytesLeft;

   // How much data we are actually going to read from memory
   size_t actualSizeToRead;

   // Our vorbis data, for the typecast
   OggBuffer* vorbisData;

   // Get the data in the right format
   vorbisData = (OggBuffer*)datasource;

   // Calculate how much we need to read.
   // This can be sizeToRead * byteSize or less depending on
   // how near the EOF marker we are
   bytesLeft = vorbisData->mBuffer->size() - vorbisData->mBytesRead;
   if ((sizeToRead * byteSize) < bytesLeft)
      actualSizeToRead = (sizeToRead * byteSize);
   else
      actualSizeToRead = bytesLeft;

   // A simple copy of the data from memory to the datastruct that the vorbis libs will use
   if (actualSizeToRead)
   {
      // Copy the data from the start of the file PLUS how much we have already read in
      memcpy(
         ptr,
         (char*)vorbisData->mBuffer + vorbisData->mBytesRead,
         actualSizeToRead
      );

      // Increase by how much we have read by
      vorbisData->mBytesRead += (actualSizeToRead);
   }

   // Return how much we read (in the same way fread would)
   return actualSizeToRead;
}


int vorbisSeek(
   void* datasource,
   ogg_int64_t offset,
   int whence
)
{
   // How much more we can read till we hit the EOF marker
   size_t bytesLeft;

   // How much we can actually offset it by
   ogg_int64_t actualOffset;

   // The data we passed in (for the typecast)
   OggBuffer* vorbisData;

   // Get the data in the right format
   vorbisData = (OggBuffer*)datasource;

   // Goto where we wish to seek to
   switch (whence)
   {
      case SEEK_SET:
      {
         if (vorbisData->mBuffer->size() >= offset)
            actualOffset = offset;
         else
            actualOffset = vorbisData->mBuffer->size();

         // Set where we now are
         vorbisData->mBytesRead = (int)actualOffset;
         break;
      }

      case SEEK_CUR:
      {
         bytesLeft =
               vorbisData->mBuffer->size()
             - vorbisData->mBytesRead;

         if (offset < bytesLeft)
            actualOffset = (offset);
         else
            actualOffset = bytesLeft;

         vorbisData->mBytesRead += actualOffset;
         break;
      }

      case SEEK_END:
      {
         vorbisData->mBytesRead = vorbisData->mBuffer->size() + 1;
         break;
      }

      default:
      {
         qWarning("unknown seek command");
         break;
      }
   };

   return 0;
}


int vorbisClose(void *datasource)
{
   return 1;
}


long vorbisTell(void *datasource)
{
   return ((OggBuffer*)datasource)->mBytesRead;
}



void OpenAlOggLoader::openFromMemory(QByteArray* array)
{
   mOggMemoryFile.mBuffer = array;
   mOggMemoryFile.mBytesRead = 0;

   // callbacks to be used to read the file from memory
   ov_callbacks callBackFunctions;

   callBackFunctions.read_func = vorbisRead;
   callBackFunctions.close_func = vorbisClose;
   callBackFunctions.seek_func = vorbisSeek;
   callBackFunctions.tell_func = vorbisTell;

   // open file from memory
   if (ov_open_callbacks(
      &mOggMemoryFile,
      &mOggStream,
      NULL,
      0,
      callBackFunctions
      ) != 0
   )
   {
      qWarning("could not read ogg file from memory");
   }

    mVorbisInfo = ov_info(&mOggStream, -1);
    mVorbisComment = ov_comment(&mOggStream, -1);

    if(mVorbisInfo->channels == 1)
        mFormat = AL_FORMAT_MONO16;
    else
        mFormat = AL_FORMAT_STEREO16;


    alGenBuffers(2, mBuffers);
    check();
    alGenSources(1, &mSource);
    check();

    alSource3f(mSource, AL_POSITION,        0.0, 0.0, 0.0);
    alSource3f(mSource, AL_VELOCITY,        0.0, 0.0, 0.0);
    alSource3f(mSource, AL_DIRECTION,       0.0, 0.0, 0.0);
    alSourcef (mSource, AL_ROLLOFF_FACTOR,  0.0          );
    alSourcei (mSource, AL_SOURCE_RELATIVE, AL_TRUE      );
}
*/
