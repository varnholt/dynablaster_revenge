#ifndef BOTACTION_H
#define BOTACTION_H

class BotAction
{
   public:

      //! action types
      enum ActionType
      {
         ActionIdle,
         ActionWalk,
         ActionBomb
      };

      //! constructor
      BotAction();

      //! destructor
      virtual ~BotAction();

      //! getter for action type
      ActionType getActionType() const;


   protected:

      //! action type
      ActionType mActionType;
};

#endif // BOTACTION_H
