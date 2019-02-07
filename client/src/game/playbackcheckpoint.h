#ifndef PLAYBACKCHECKPOINT_H
#define PLAYBACKCHECKPOINT_H

class PlaybackCheckpoint
{
   public:

      //! constructor
      PlaybackCheckpoint();

      //! setter for elapsed time
      void setElapsed(int elapsed);

      //! getter for elapsed time
      int getElapsed() const;

      //! setter for visited flag
      void setVisited(bool visited);

      //! getter for visited flag
      bool isVisited() const;


   protected:

      //! elapsed time
      int mElapsed;

      //! visited flag
      bool mVisited;
};

#endif // PLAYBACKCHECKPOINT_H
