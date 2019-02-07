#ifndef GAMESTATEMACHINE_H
#define GAMESTATEMACHINE_H

// Qt
#include <QObject>

// shared
#include "constants.h"


class GameStateMachine : public QObject
{
   Q_OBJECT

   public:

      static GameStateMachine* getInstance();

      void setState(Constants::GameState nextState);

      Constants::GameState getState() const;


   signals:

      void stateChanged();


   protected:

      GameStateMachine();

      Constants::GameState mState;

      static GameStateMachine* sInstance;

};

#endif // GAMESTATEMACHINE_H
