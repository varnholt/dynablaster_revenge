#ifndef BOTBOMBACTION_H
#define BOTBOMBACTION_H

#include "botaction.h"
#include "constants.h"

class BotBombAction : public BotAction
{
   public:

      //! constructor
      BotBombAction();

      //! action is combinable
      virtual bool isCombinable() const;


   protected:

};

#endif // BOTBOMBACTION_H
