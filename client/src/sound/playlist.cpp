// header
#include "playlist.h"

// Qt
#include <QThreadPool>
#include <QTime>

// soundsystem
#include "../soundsystem/streamplayer.h"

// mp3
#include "../mp3/id3.h"

// tools
#include "tools/filestreamrunnable.h"


Playlist::Playlist(QObject *parent)
   : QObject(parent),
     mMixer(0),
     mShuffle(false),
     mIndex(0),
     mBuffer(0)
{
}


Playlist::~Playlist()
{
   delete[] mBuffer;
}


void Playlist::setMixer(StreamPlayer* mixer)
{
   mMixer = mixer;

   connect(
      mixer,
      SIGNAL(finished()),
      this,
      SLOT(next()),
      Qt::QueuedConnection
   );
}


void Playlist::setPlaylist(const QList<QString>& playlist)
{
   mPlaylist = playlist;
}


void Playlist::setShuffleEnabled(bool enabled)
{
   mShuffle = enabled;

   // initially we want a random track
   if (enabled)
   {
      if (mPlaylist.size() > 0)
      {
         qsrand(QTime::currentTime().msec());
         mIndex = (qrand() % mPlaylist.size());
      }
   }
}


bool Playlist::isShuffleEnabled() const
{
   return mShuffle;
}


void Playlist::play()
{
   if (!mPlaylist.isEmpty())
   {
       qDebug(
          "Playlist: playing %s",
          qPrintable(mPlaylist.at(mIndex))
       );

       if (mMixer)
       {
          QString filename= mPlaylist.at(mIndex);

          FilestreamRunnable* runnable = new FilestreamRunnable();
          runnable->setFilename(filename);

          connect(
             runnable,
             SIGNAL(loaded(char*,int)),
             this,
             SLOT(playBuffer(char*,int)),
             Qt::QueuedConnection
          );

          connect(
             runnable,
             SIGNAL(loaded(char*,int)),
             runnable,
             SLOT(deleteLater()),
             Qt::QueuedConnection
          );

          QThreadPool::globalInstance()->start(runnable);
       }
   }
}


void Playlist::playBuffer(char* buffer, int size)
{
   mMixer->play( buffer, size );

   // delete previous data
   if (mBuffer)
      delete[] mBuffer;

   mBuffer= buffer;

   // id3 tag is now available
   const Id3Tag& id3 = mMixer->getId3();

   emit playing(
      id3.artist(),
      id3.album(),
      id3.title()
   );
}


void Playlist::pause()
{
   if (mMixer)
   {
      mMixer->pause();
   }
}


void Playlist::stop()
{
   if (mMixer)
   {
      mMixer->stop();
   }
}


void Playlist::playNext()
{
   next();
}


void Playlist::playPrevious()
{
   previous();
}


void Playlist::next()
{
   if (mPlaylist.size() > mIndex)
   {
      // QString track= mPlaylist.at(mIndex);
      mPlayedTracks.insert(mPlaylist.at(mIndex));
   }

   // if shuffle pick the next unplayed track
   // otherwise just go on looping the playlist
   int nextIndex = mIndex;

   if (mShuffle)
   {
      nextIndex = shuffle(nextIndex);
   }
   else
   {
      nextIndex++;

      if (nextIndex >= mPlaylist.size())
         nextIndex = 0;
   }

   // assign the next index to play
   mIndex = nextIndex;

   // play next track
   play();
}


void Playlist::previous()
{
   mPlayedTracks.insert(mPlaylist.at(mIndex));

   // if shuffle pick the next unplayed track
   // otherwise just go on looping the playlist
   int nextIndex = mIndex;

   if (mShuffle)
   {
      nextIndex = shuffle(nextIndex);
   }
   else
   {
      nextIndex--;

      if (nextIndex < 0)
         nextIndex = mPlaylist.size() - 1;
   }

   // assign the next index to play
   mIndex = nextIndex;

   // play next track
   play();
}


int Playlist::shuffle(int nextIndex)
{
   // clear list of played tracks after all tracks have been played
   if (mPlayedTracks.size() == mPlaylist.size())
      mPlayedTracks.clear();

   while (!mPlaylist.isEmpty() && mPlayedTracks.contains(mPlaylist.at(nextIndex)))
      nextIndex = qrand() % mPlaylist.size();

   return nextIndex;
}



