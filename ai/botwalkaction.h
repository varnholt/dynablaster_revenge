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
      void setWalkKeys(int8_t);

      //! getter for walk direction
      int8_t getWalkKeys() const;


   protected:

      //! walk direction
      int8_t mWalkKeys;


      // Path or target point
};

#endif // BOTWALKACTION_H
