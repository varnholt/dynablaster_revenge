#ifndef GAMEROUND_H
#define GAMEROUND_H

class GameRound
{
   public:

      GameRound();

      void reset();

      void next();

      bool isFinished() const;

      int getCurrent() const;

      int getCount() const;

      void setCurrent(int current);

      void setCount(int count);


   protected:

      int mCurrent;

      int mCount;
};

#endif // GAMEROUND_H
