#ifndef GAMEMESSAGE_H
#define GAMEMESSAGE_H

#include "framework/frametimer.h"

// Qt
#include <QString>

class GameMessage : public QObject
{
   Q_OBJECT

   public:

      //! constructor
      GameMessage(QObject* parent = 0);

      void operator=(const GameMessage&);
      GameMessage(const GameMessage& message);

      //! setter for the message
      void setMessage(const QString&);

      //! setter for message display time (in ms)
      static void setDisplayTime(int time);

      virtual void initialize() {};


   signals:

      //! message has expired
      void expired();


   protected:

      //! message
      QString mMessage;

      //! message send time
      FrameTimer mTime;

      //! id of the sender
      int mSenderId;

      //! id of receiver
      int mReceiverId;

      //! name of the sender
      QString mSenderName;

      //! name of the receiver
      QString mReceiverName;

      //! message display time
      static int sDisplayTime;

};

#endif // GAMEMESSAGE_H
