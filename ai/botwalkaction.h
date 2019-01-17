#ifndef BOTWALKACTION_H
#define BOTWALKACTION_H

// Qt
#include "QtGlobal"

// bot
#include "botaction.h"

// shared
#include "constants.h"

class BotWalkAction : public BotAction
{
   public:

      //! constructor
      BotWalkAction();

      //! setter for walk direction
      void setWalkKeys(qint8);

      //! getter for walk direction
      qint8 getWalkKeys() const;


   protected:

      //! walk direction
      qint8 mWalkKeys;


      // Path or target point
};

#endif // BOTWALKACTION_H
