#ifndef PROTOBOTMEMORY_H
#define PROTOBOTMEMORY_H

class ProtoBotMemory
{
   public:

      //! constructor
      ProtoBotMemory();

      //! reset memory
      void reset();


      // extras

      //! invalidate last extra position
      void invalidateExtraPosition();

      //! setter for extra position
      void setExtraPosition(int x, int y);

      //! check if extra position is valid
      bool isExtraPositionValid() const;

      //! getter for extra position x
      int getExtraPositionX() const;

      //! getter for extra position y
      int getExtraPositionY() const;


      // stones

      //! invalidate last bomb stone position
      void invalidateBombStonePosition();

      //! setter for bomb stone position
      void setBombStonePosition(int x, int y);

      //! check if bomb stone position is valid
      bool isBombStonePositionValid() const;

      //! getter for bomb stone position x
      int getBombStonePositionX() const;

      //! getter for bomb stone position y
      int getBombStonePositionY() const;

      //! setter for stone count around position
      void setBombStoneCount(int count);

      //! getter for bomb stone neighbor count
      int getBombStoneCount() const;


   protected:

      // extras

      //! last extra x position
      int mExtraX;

      //! last extra y position
      int mExtraY;


      // stones

      //! last bomb stone x position
      int mBombStoneX;

      //! last bomb stone y position
      int mBombStoneY;

      //! stones located around that position
      int mBombStoneCount;
};

#endif // PROTOBOTMEMORY_H
