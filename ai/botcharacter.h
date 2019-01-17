#ifndef BOTCHARACTER_H
#define BOTCHARACTER_H

class BotCharacter
{
   public:

      //! constructor
      BotCharacter();

      void setCharacter(int extras, int bombdrop, int attack);

      int getScoreForExtras() const;
      int getScoreForPrepareBombDrop() const;
      int getScoreForAttack() const;


   protected:

      int mScoreForEscape;
      int mScoreForExtras;
      int mScoreForPrepareBombDrop;
      int mScoreForAttack;

};

#endif // BOTCHARACTER_H
