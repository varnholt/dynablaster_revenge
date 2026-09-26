#ifndef BOTBOMBMAPITEM_H
#define BOTBOMBMAPITEM_H

// base
#include "bombmapitem.h"

#include <chrono>

class BotBombMapItem : public BombMapItem
{
   Q_OBJECT

   public:

      //! constructor
      BotBombMapItem(
         int playerId,
         int flames,
         int id,
         int x,
         int y
      );

      //! getter for drop time
      std::chrono::steady_clock::time_point getDropTime() const;

      //! setter for flame count
      void setFlameCount(int flames);


   protected:

       //! bomb drop time
       std::chrono::steady_clock::time_point mDropTime;
};

#endif // BOTBOMBMAPITEM_H
