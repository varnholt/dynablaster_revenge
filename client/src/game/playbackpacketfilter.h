#ifndef PLAYBACKPACKETFILTER_H
#define PLAYBACKPACKETFILTER_H

class PlaybackPacketFilter
{
   public:

      //! constructor
      PlaybackPacketFilter();

      //! setter for game id
      void setGameId(int id);

      //! getter for game id
      int getGameId() const;

      //! setter for round number
      void setRoundNumber(int number);

      //! getter for round number
      int getRoundNumber() const;

      //! check if filter is enabled
      bool isEnabled() const;

      //! setter for enabled flag
      void setEnabled(bool enabled);

      //! setter for running flag
      void setRunning(bool running);

      //! getter for running flag
      bool isRunning() const;

      //! check if filter is valid
      bool isValid() const;

      //! increase round counter
      void increaseRoundCounter();

      //! getter for round counter
      int getRoundCounter() const;

      //! check if round is reached
      bool isRoundReached() const;

      //! reset filter
      void reset();


   protected:

      //! selected game number
      int mGameId;

      //! selected round number
      int mRoundNumber;

      //! enabled flag
      bool mEnabled;

      //! game running flag
      bool mRunning;

      //! round counter
      int mRoundCounter;
};

#endif // PLAYBACKPACKETFILTER_H
