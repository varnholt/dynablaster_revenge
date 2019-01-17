#ifndef LISTGAMESRESPONSEPACKET_H
#define LISTGAMESRESPONSEPACKET_H

#include "packet.h"

// shared
#include "gameinformation.h"

// Qt
#include <QList>
#include <QString>


class ListGamesResponsePacket : public Packet
{

   public:

      //! write constructor
      ListGamesResponsePacket(
         const QList<GameInformation>& games,
         bool update = false
      );

      //! read constructor
      ListGamesResponsePacket();

      //! destructor
      virtual ~ListGamesResponsePacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

      //! getter for game name
      const QList<GameInformation> getGames();

      //! setter for update flag
      void setUpdate(bool update);

      //! getter for update flag
      bool isUpdate() const;


   private:

      //! game's name
      QList<GameInformation> mGames;

      //! update game information
      bool mUpdate;
};

#endif // LISTGAMESRESPONSEPACKET_H
