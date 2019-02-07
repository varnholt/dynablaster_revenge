#ifndef GAMEMENUINTERFACELOUNGE_H
#define GAMEMENUINTERFACELOUNGE_H

// base
#include "gamemenuinterface.h"

// game
#include "playerinfo.h"

// Qt
#include <QTimer>

struct QPairFirstComparer
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
    {
        return a.first > b.first;
    }
};

struct QPairSecondComparer
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
    {
        return a.second > b.second;
    }
};

class GameMenuInterfaceLounge : public GameMenuInterface
{
   Q_OBJECT

public:

   //! constructor
   GameMenuInterfaceLounge(QObject* parent = 0);

   //! getter for lounge message
   QString getLoungeMessage() const;

   //! clear lounge message that was just sent
   void clearLoungeMessage();

   //! clear lounge messages
   void clearLoungeMessages();

   //! add a message to the lounge
   void addLoungeMessage(
      int senderId,
      QString message
   );

   //! update player is typing flag
   void updatePlayerTyping(
      int senderId,
      bool typing
   );

   //! update player list
   void playerInfoMapUpdated(QMap<int, PlayerInfo*>* playerInfo);

   //! reset ui states on show
   void initializeLoungeStates();

   //! enable or disable update timer
   void setLoungeConnectionsEnabled(bool);

   //! pick the last message from the client and set it to the lineedit
   void initializeLastPlayerMessage();

   //! update the add headless player button
   void updateAddHeadlessPlayerButton();


public slots:

   //! update start game button
   void gameInfoUpdated();


private slots:

   //! remove a typing icon
   void removePlayerTyping();


private:

   //! mapping from player id to list index
   QMap<int, int> mPlayerIdToIndexMap;

   //! messages
   QMap<int, QTimer*> mPlayersTyping;
};

#endif // GAMEMENUINTERFACELOUNGE_H
