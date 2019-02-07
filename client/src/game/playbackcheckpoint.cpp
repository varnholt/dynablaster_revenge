#include "playbackcheckpoint.h"


PlaybackCheckpoint::PlaybackCheckpoint()
  : mElapsed(0),
    mVisited(false)
{
}


void PlaybackCheckpoint::setElapsed(int elapsed)
{
   mElapsed = elapsed;
}


int PlaybackCheckpoint::getElapsed() const
{
   return mElapsed;
}


void PlaybackCheckpoint::setVisited(bool visited)
{
   mVisited = visited;
}


bool PlaybackCheckpoint::isVisited() const
{
   return mVisited;
}

