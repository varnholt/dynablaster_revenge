#ifndef BOTBOMBMAPITEM_H
#define BOTBOMBMAPITEM_H

// base
#include "bombmapitem.h"

// Qt
#include <QTime>

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
      const QTime& getDropTime() const;

      //! setter for flame count
      void setFlameCount(int flames);


   protected:

       //! bomb drop time
       QTime mDropTime;
};

#endif // BOTBOMBMAPITEM_H
